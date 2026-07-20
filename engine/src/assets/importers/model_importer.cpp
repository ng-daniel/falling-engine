#include "engine/assets/asset_data.h"

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "engine/assets/asset_helpers.h"
#include "engine/assets/importers/model_importer.h"
#include "engine/assets/importers/image_importer.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

std::vector<unsigned char> DecodeDataUri(const char* uri);

/**
 * @brief Loads a GLTF model asset from the specified path.
 * 
 * @param path The file path to the model asset.
 * @return A vector of unique pointers to Asset objects representing the loaded model.
 */
std::vector<std::unique_ptr<Asset>>
ModelImporter::LoadAsset(SourceAssetMetadata& metadata, AssetWarehouseService& assetWarehouseService) {
    ModelImportContext modelImportContext(assetWarehouseService, metadata.path.parent_path());
    std::vector<std::unique_ptr<Asset>> importedAssets;
    return importedAssets;

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

    for (cgltf_size i = 0; i < data->textures_count; ++i)
    {
        cgltf_texture * texture = &data->textures[i];
        std::unique_ptr<TextureAsset> textureAsset = ProcessTexture(*texture, modelImportContext);
        modelImportContext.importedTextures[texture] = textureAsset->id;
        importedAssets.push_back(std::move(textureAsset));
    }
    
    // process all materials in the model

    for (cgltf_size i = 0; i < data->materials_count; ++i)
    {
        cgltf_material * material = &data->materials[i];
        std::unique_ptr<MaterialAsset> materialAsset = ProcessMaterial(*material, modelImportContext);
        modelImportContext.importedMaterials[material] = materialAsset->id;
        importedAssets.push_back(std::move(materialAsset));
    }

    // process all meshes in the model

    for (cgltf_size i = 0; i < data->meshes_count; ++i)
    {
        cgltf_mesh * mesh = &data->meshes[i];
        std::unique_ptr<MeshAsset> meshAsset = ProcessMesh(*mesh, modelImportContext);
        modelImportContext.importedMeshes[mesh] = meshAsset->id;
        importedAssets.push_back(std::move(meshAsset));
    }

    cgltf_free(data);

    return importedAssets;
}

/**
 * @brief Processes a GLTF image and returns a unique pointer to an ImageAsset.
 * 
 * @param image The CGLTF image structure to process.
 * @param importData Reference structure containing cgltf ptrs -> asset id mappings.
 * @return std::unique_ptr<ImageAsset> 
 */
std::unique_ptr<ImageAsset> ModelImporter::ProcessImage(const cgltf_image& image, ModelImportContext& modelImportContext) {
    UUID imageId = UUIDGenerator::GenerateUUID();
    std::string imageName;
    if (image.name != nullptr && image.name[0] != '\0') {
        imageName = image.name;
    } else {
        imageName = "image_" + UUIDToString(imageId);
    }

    /*
    3 valid cases + 1 invalid case:
    
    case 1: glb binary, image is already embedded as binary data (create new metadata)
    case 2: gltf encoded, image is embedded as base64 data URI (create new metadata)
    case 3: gltf separate, image is a separate file referenced by URI (sometimes new, sometimes existing asset)
    else: ERROR
    */

    std::vector<std::unique_ptr<Asset>> importedAssets;
    if (image.buffer_view != nullptr) {
        const uint8_t* bufferData = cgltf_buffer_view_data(image.buffer_view);
        if (bufferData == nullptr) {
            throw std::runtime_error("Failed to access embedded image buffer view.");
        }
        std::vector<unsigned char> encodedImageData(
            bufferData,
            bufferData + image.buffer_view->size
        );
        importedAssets = ImageImporter::LoadAssetFromMemory(encodedImageData);
    }
    else if (image.uri != nullptr && image.uri[0] != '\0'&& strncmp(image.uri, "data:", 5) == 0) {
        std::vector<unsigned char> decodedImageData = DecodeDataUri(image.uri);
        importedAssets = ImageImporter::LoadAssetFromMemory(decodedImageData);
    }
    else if (image.uri != nullptr && image.uri[0] != '\0') {
        std::filesystem::path imagePath = modelImportContext.modelDirectory / image.uri;
        SourceAssetMetadata imageMetadata {
            .id = imageId,
            .type = "Image",
            .path = imagePath,
            .assetMetadatas = {},
            .loaded = false,
        };
        importedAssets = ImageImporter::LoadAsset(imageMetadata);
    }
    else {
        throw std::runtime_error("GLTF image must provide either a buffer view or URI.");
    }

    // ensure that exactly one image asset was produced

    if (importedAssets.size() != 1 || importedAssets.front() == nullptr) {
        throw std::runtime_error("Image import did not produce exactly one asset.");
    }
    auto imageAsset = std::unique_ptr<ImageAsset>(
        static_cast<ImageAsset*>(importedAssets.front().release())
    );
    ApplyMetadataToAsset(imageId, imageName, "Image", *imageAsset);

    return imageAsset;
}

std::unique_ptr<TextureAsset> ModelImporter::ProcessTexture(const cgltf_texture& texture, ModelImportContext& modelImportContext) {
    return nullptr;
}

std::unique_ptr<MaterialAsset> ModelImporter::ProcessMaterial(const cgltf_material& material, ModelImportContext& modelImportContext) {
    return nullptr;
}

std::unique_ptr<MeshAsset> ModelImporter::ProcessMesh(const cgltf_mesh& mesh, ModelImportContext& modelImportContext) {
    return nullptr;
}

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