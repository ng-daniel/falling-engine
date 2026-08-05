#include "engine/assets/asset_data.h"

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "engine/assets/asset_helpers.h"
#include "engine/assets/importers/model_importer.h"
#include "engine/assets/importers/image_importer.h"
#include "engine/assets/importers/texture_importer.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

/**
 * @brief HELPER: Decodes a data URI and returns the decoded image data as a vector of unsigned chars.
 * 
 * @param uri the data URI to decode.
 * @return std::vector<unsigned char> 
 */
std::vector<unsigned char> DecodeDataUri(const char* uri) {
    
    // validate URI format
    const char* comma = strchr(uri, ',');
    if (comma == nullptr || comma - uri < 7 || strncmp(comma - 7, ";base64", 7) != 0) {
        throw std::runtime_error("Unsupported data URI image format.");
    }

    // validate base64 payload
    const char* base64Data = comma + 1;
    size_t base64Length = strlen(base64Data);
    if (base64Length == 0 || base64Length % 4 != 0) {
        throw std::runtime_error("Malformed base64 image payload.");
    }

    // compute decoded size
    size_t padding = 0;
    if (base64Length >= 1 && base64Data[base64Length - 1] == '=') {
        ++padding;
    }
    if (base64Length >= 2 && base64Data[base64Length - 2] == '=') {
        ++padding;
    }
    size_t decodedSize = (base64Length / 4) * 3 - padding;

    // decode base64 data using cgltf's base64 decoder
    cgltf_options options {};
    void * decodedData = nullptr;
    cgltf_result decodeResult = cgltf_load_buffer_base64(&options, decodedSize, base64Data, &decodedData);
    if (decodeResult != cgltf_result_success || decodedData == nullptr) {
        throw std::runtime_error("Failed to decode data URI image payload.");
    }

    // convert to unsigned char vector format
    unsigned char * bytes = static_cast<unsigned char *>(decodedData);
    std::vector<unsigned char> imageData(bytes, bytes + decodedSize);

    std::free(decodedData);
    return imageData;
}

std::string BuildSubAssetNameFromGLTFName(const std::string&glTFName, Asset::AssetType assetType) {
    return GetStringFromAssetType(assetType) + "__" + glTFName;
}

/**
 * @brief Loads a GLTF model asset from the specified path.
 * 
 * @param path The file path to the model asset.
 * @return A vector of stored asset pointers representing the loaded model.
 */
std::vector<const Asset*>
ModelImporter::LoadAsset(SourceAssetMetadata& metadata, AssetWarehouseService& assetWarehouseService) {
    std::vector<const Asset*> importedAssets;
    ModelImportContext modelImportContext(
        metadata,
        assetWarehouseService,
        metadata.path.parent_path()
    );

    // parse file

    cgltf_file_type type = metadata.path.extension() == ".gltf" ? cgltf_file_type_gltf : cgltf_file_type_glb;
    std::string gltf_path = metadata.path.string();

    cgltf_options options = {type};
    cgltf_data * data = NULL;
    cgltf_result result = cgltf_parse_file(&options, 
        gltf_path.c_str(), &data);
    if (result != cgltf_result_success)
    {
        throw std::runtime_error("Failed to parse glTF file.");
        return importedAssets;
    }
    result = cgltf_load_buffers(&options, data,
        gltf_path.c_str());
    if (result != cgltf_result_success) {
        cgltf_free(data);
        throw std::runtime_error("Failed to load glTF buffers.");
        return importedAssets;
    }

    // process all assets in the model

    for (cgltf_size i = 0; i < data->images_count; ++i)
    {
        cgltf_image * image = &data->images[i];
        const std::string imageName = image->name
            ? image->name
            : "image_" + std::to_string(i);
        const ImageAsset* imageAsset = ProcessImage(*image, imageName, modelImportContext);
        if (imageAsset) {
            modelImportContext.importedImages[image] = imageAsset->id;
            importedAssets.push_back(imageAsset);
        }
    }

    for (cgltf_size i = 0; i < data->textures_count; ++i)
    {
        cgltf_texture * texture = &data->textures[i];
        const TextureAsset* textureAsset = ProcessTexture(*texture, modelImportContext);
        if (textureAsset != nullptr) {
            modelImportContext.importedTextures[texture] = textureAsset->id;
            importedAssets.push_back(textureAsset);
        }
    }
    
    // // process all materials in the model

    // for (cgltf_size i = 0; i < data->materials_count; ++i)
    // {
    //     cgltf_material * material = &data->materials[i];
    //     std::unique_ptr<MaterialAsset> materialAsset = ProcessMaterial(*material, modelImportContext);
    //     modelImportContext.importedMaterials[material] = materialAsset->id;
    //     importedAssets.push_back(std::move(materialAsset));
    // }

    // // process all meshes in the model

    // for (cgltf_size i = 0; i < data->meshes_count; ++i)
    // {
    //     cgltf_mesh * mesh = &data->meshes[i];
    //     std::unique_ptr<MeshAsset> meshAsset = ProcessMesh(*mesh, modelImportContext);
    //     modelImportContext.importedMeshes[mesh] = meshAsset->id;
    //     importedAssets.push_back(std::move(meshAsset));
    // }

    cgltf_free(data);

    return importedAssets;
}

/**
 * @brief Processes a GLTF image and returns a stored ImageAsset pointer.
 * 
 * @param image The CGLTF image structure to process.
 * @param importData Reference structure containing cgltf ptrs -> asset id mappings.
 * @return const ImageAsset* 
 *
 * @note Storage happens inside the dependent importers so the returned asset
 * already has its runtime UUID assigned and can be referenced immediately.
 */
const ImageAsset* ModelImporter::ProcessImage(
    const cgltf_image& image,
    const std::string& imageName,
    const ModelImportContext& modelImportContext
) {
    /*
    3 valid cases + 1 invalid case:
    
    case 1: glb binary, image is already embedded as binary data (create new metadata)
    case 2: gltf encoded, image is embedded as base64 data URI (create new metadata)
    case 3: gltf separate, image is a separate file referenced by URI (sometimes new, sometimes existing asset)
    else: ERROR
    */

    std::string subAssetIdentifier = BuildSubAssetNameFromGLTFName(
        imageName,
        Asset::AssetType::Image
    );
    RuntimeAssetMetadata* imageRuntimeMetadata =
        modelImportContext.sourceAssetMetadata.TryGetSubAssetMetadata(subAssetIdentifier);

    if (image.buffer_view != nullptr) {
        /*
        case 1
        read raw bytes from buffer and load the image asset from memory
        */
        const uint8_t* bufferData = cgltf_buffer_view_data(image.buffer_view);
        if (bufferData == nullptr) {
            throw std::runtime_error("Failed to access embedded image buffer view.");
        }
        std::vector<unsigned char> encodedImageData(
            bufferData,
            bufferData + image.buffer_view->size
        );
        return ImageImporter::LoadAssetFromMemory(
            modelImportContext.sourceAssetMetadata,
            modelImportContext.assetWarehouseService,
            encodedImageData,
            subAssetIdentifier,
            imageRuntimeMetadata
        );
    }
    else if (image.uri != nullptr && image.uri[0] != '\0'&& strncmp(image.uri, "data:", 5) == 0) {
        /*
        case 2
        decode the base64 data URI and load the image asset from memory
        */
        std::vector<unsigned char> decodedImageData = DecodeDataUri(image.uri);
        return ImageImporter::LoadAssetFromMemory(
            modelImportContext.sourceAssetMetadata,
            modelImportContext.assetWarehouseService,
            decodedImageData,
            subAssetIdentifier,
            imageRuntimeMetadata
        );
    }
    else if (image.uri != nullptr && image.uri[0] != '\0') {
        /*
        case 3
        check if the source image is already loaded
        if not, load it from the file path
        */
        std::filesystem::path imagePath = modelImportContext.modelDirectory / image.uri;
        SourceAssetMetadata imageMetadata;
        try {
            imageMetadata = modelImportContext.assetWarehouseService.DependencyResolver(imagePath);
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to resolve dependency for image asset: " + imagePath.string() + ". Error: " + e.what());
        }

        if (imageMetadata.loaded) {
            // pull from warehouse if already loaded

            RuntimeAssetMetadata* runtimeMetadata = imageMetadata.GetPrimaryRuntimeMetadata();
            if (runtimeMetadata == nullptr) {
                throw std::runtime_error("Loaded image dependency is missing runtime metadata: " + imagePath.string());
            }

            const Asset* loadedAsset = modelImportContext.assetWarehouseService.GetLoadedAssetReadOnly(runtimeMetadata->id);
            if (loadedAsset == nullptr) {
                throw std::runtime_error("Loaded image dependency is missing warehouse asset: " + imagePath.string());
            }
            return static_cast<const ImageAsset*>(loadedAsset);
        }
        else {
            // load from file if not already loaded

            // since the image is also a source, we need to create both the image and a default texture asset for it
            // which is why we need to call the TextureImporter to load the image and create a texture asset for it
            // then query the image asset from the warehouse using the texture asset's image UUID

            const TextureAsset* textureAsset = TextureImporter::LoadAsset(imageMetadata, modelImportContext.assetWarehouseService);
            return static_cast<const ImageAsset*>(
                modelImportContext.assetWarehouseService.GetLoadedAssetReadOnly(textureAsset->image)
            );
        }
    }
    else {
        throw std::runtime_error("GLTF image must provide either a buffer view or URI.");
    }
}

const TextureAsset* ModelImporter::ProcessTexture(const cgltf_texture& texture, const ModelImportContext& modelImportContext) {
    if (texture.image == nullptr) {
        throw std::runtime_error("Texture is missing an image reference.");
    }

    UUID imageId;
    auto imageIt = modelImportContext.importedImages.find(texture.image);
    if (imageIt != modelImportContext.importedImages.end()) {
        imageId = imageIt->second;
    } else {
        const std::string imageName = texture.image->name
            ? texture.image->name
            : "texture_image";
        const ImageAsset* imageAsset = ProcessImage(*texture.image, imageName, modelImportContext);
        if (imageAsset == nullptr) {
            throw std::runtime_error("Failed to import texture image dependency.");
        }
        imageId = imageAsset->id;
    }

    const std::string textureName = texture.name != nullptr && texture.name[0] != '\0'
        ? texture.name
        : (texture.image->name != nullptr && texture.image->name[0] != '\0'
            ? texture.image->name
            : "texture");
    const std::string subAssetIdentifier = BuildSubAssetNameFromGLTFName(
        textureName,
        Asset::AssetType::Texture
    );

    const TextureAsset* textureAsset = TextureImporter::LoadAssetFromImage(
        modelImportContext.sourceAssetMetadata,
        modelImportContext.assetWarehouseService,
        imageId,
        subAssetIdentifier
    );

    if (texture.sampler != nullptr) {
        Asset* storedAsset = modelImportContext.assetWarehouseService.GetLoadedAsset(textureAsset->id);
        if (storedAsset == nullptr) {
            throw std::runtime_error("Texture asset was not stored in the warehouse.");
        }

        TextureAsset* mutableTextureAsset = static_cast<TextureAsset*>(storedAsset);
        mutableTextureAsset->mag_filter = static_cast<TextureAsset::FilterType>(texture.sampler->mag_filter);
        mutableTextureAsset->min_filter = static_cast<TextureAsset::FilterType>(texture.sampler->min_filter);
        mutableTextureAsset->wrap_s = static_cast<TextureAsset::WrapMode>(texture.sampler->wrap_s);
        mutableTextureAsset->wrap_t = static_cast<TextureAsset::WrapMode>(texture.sampler->wrap_t);
    }

    return textureAsset;
}

const MaterialAsset* ModelImporter::ProcessMaterial(const cgltf_material& material, const ModelImportContext& modelImportContext) {
    return nullptr;
}

const MeshAsset* ModelImporter::ProcessMesh(const cgltf_mesh& mesh, const ModelImportContext& modelImportContext) {
    return nullptr;
}