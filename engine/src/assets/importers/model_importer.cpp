#include "engine/assets/asset_data.h"

#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "engine/assets/importers/model_importer.h"
#include "engine/assets/importers/image_importer.h"
#include "engine/assets/importers/texture_importer.h"
#include "engine/debug/logger.h"

#include "engine/utils/quaternion.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define CGLTF_IMPLEMENTATION
#include "external/cgltf.h"

namespace {

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

    std::vector<std::string> invalidCharacters = { 
        " ", 
        "/", 
        "\\",
        ":",
        "*",
        "?",
        "\"",
        "<", 
        ">", 
        "|", 
        "."
    }; 


    std::string BuildSubAssetNameFromGLTFName(const std::string& sourceName, const std::string& glTFName, Asset::AssetType assetType) {
        // replace invalid characters with underscores

        std::string normalizedGLTFName = glTFName;
        for (const auto& invalidChar : invalidCharacters) {
            size_t pos = 0;
            while ((pos = normalizedGLTFName.find(invalidChar, pos)) != std::string::npos) {
                normalizedGLTFName.replace(pos, invalidChar.length(), "_");
                pos += 1;
            }
        }
        return sourceName + "_" + normalizedGLTFName;
    }
}

/**
 * @brief Loads a GLTF model asset from the specified path.
 * 
 * @param path The file path to the model asset.
 * @return A vector of stored asset pointers representing the loaded model.
 */
const ModelAsset *
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
        return nullptr;
    }
    result = cgltf_load_buffers(&options, data,
        gltf_path.c_str());
    if (result != cgltf_result_success) {
        cgltf_free(data);
        throw std::runtime_error("Failed to load glTF buffers.");
        return nullptr;
    }
    modelImportContext.parsedData = data;

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
        if (textureAsset) {
            modelImportContext.importedTextures[texture] = textureAsset->id;
            importedAssets.push_back(textureAsset);
        }
    }

    for (cgltf_size i = 0; i < data->materials_count; ++i)
    {
        cgltf_material * material = &data->materials[i];
        const std::string materialName = material->name != nullptr && material->name[0] != '\0'
            ? material->name
            : "material_" + std::to_string(i);
        const MaterialAsset* materialAsset = ProcessMaterial(*material, materialName, modelImportContext);
        if (materialAsset) {
            modelImportContext.importedMaterials[material] = materialAsset->id;
            importedAssets.push_back(materialAsset);
        }
    }

    for (cgltf_size i = 0; i < data->meshes_count; ++i)
    {
        cgltf_mesh * mesh = &data->meshes[i];
        const MeshAsset* meshAsset = ProcessMesh(*mesh, modelImportContext);
        if (meshAsset) {
            modelImportContext.importedMeshes[mesh] = meshAsset->id;
            importedAssets.push_back(meshAsset);
        }
    }

    // compile all assets into a single model asset
    std::unique_ptr<ModelAsset> modelAsset = CompileModelAsset(*data->scenes, modelImportContext);

    // store it in the warehouse

    const ModelAsset * storedModelAsset = static_cast<const ModelAsset*>(
        assetWarehouseService.StoreAsset(metadata, std::move(modelAsset))
    );
    importedAssets.push_back(storedModelAsset);

    cgltf_free(data);
    return storedModelAsset;

    // FINALLY DONE :)
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
        modelImportContext.sourceAssetMetadata.path.stem().string(),
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
        SourceAssetMetadata * imageMetadata;
        imageMetadata = modelImportContext.assetWarehouseService.DependencyResolver(imagePath);

        if (imageMetadata && imageMetadata->loaded) {
            // pull from warehouse if already loaded

            const RuntimeAssetMetadata * runtimeMetadata = SourceAssetMetadata::GetPrimaryRuntimeMetadata(*imageMetadata);
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

            const TextureAsset* textureAsset = TextureImporter::LoadAsset(*imageMetadata, modelImportContext.assetWarehouseService);
            return static_cast<const ImageAsset*>(
                modelImportContext.assetWarehouseService.GetLoadedAssetReadOnly(textureAsset->image)
            );
        }
    }
    else {
        throw std::runtime_error("GLTF image must provide either a buffer view or URI.");
    }
}

/**
 * @brief Imports a GLTF texture and returns a stored TextureAsset pointer.
 * 
 * @param texture The CGLTF texture structure to process.
 * @param modelImportContext The context containing mappings and metadata for the import process.
 * @return const TextureAsset* A pointer to the imported TextureAsset.
 */
const TextureAsset* ModelImporter::ProcessTexture(const cgltf_texture& texture, const ModelImportContext& modelImportContext) {
    if (texture.image == nullptr) {
        throw std::runtime_error("Texture is missing an image reference.");
    }

    // all images must have already been imported, pull the img from the mapping
    UUID imageId;
    auto imageIt = modelImportContext.importedImages.find(texture.image);
    if (imageIt != modelImportContext.importedImages.end()) {
        imageId = imageIt->second;
        Logger::Info(
            "ModelImporter::ProcessTexture",
            "Using already imported image asset with ID: " + std::to_string(imageId)
        );
    } else {
        throw std::runtime_error("Texture image has not been imported yet. Ensure that images are processed before textures.");
    }

    const std::string textureName = texture.name != nullptr && texture.name[0] != '\0'
        ? texture.name
        : (texture.image->name != nullptr && texture.image->name[0] != '\0'
            ? texture.image->name
            : "texture");
    const std::string subAssetIdentifier = BuildSubAssetNameFromGLTFName(
        modelImportContext.sourceAssetMetadata.path.stem().string(),
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

/**
 * @brief Processes a GLTF material and returns a stored MaterialAsset pointer.
 * 
 * @param material The CGLTF material structure to process.
 * @param materialName
 * @param modelImportContext 
 * @return const MaterialAsset* 
 */
const MaterialAsset* ModelImporter::ProcessMaterial(
    const cgltf_material& material,
    const std::string& materialName,
    ModelImportContext& modelImportContext
) {
    auto materialAsset = std::make_unique<MaterialAsset>();
    materialAsset->name = BuildSubAssetNameFromGLTFName(
        modelImportContext.sourceAssetMetadata.path.stem().string(),
        materialName,
        Asset::AssetType::Material
    );
    materialAsset->type = Asset::AssetType::Material;

    if (material.unlit) {
        materialAsset->materialType = MaterialAsset::MaterialType::Unlit;
    } else {
        materialAsset->materialType = MaterialAsset::MaterialType::PBRMetallicRoughness;
    }

    if (material.has_pbr_metallic_roughness) {
        const cgltf_pbr_metallic_roughness& pbr = material.pbr_metallic_roughness;
        materialAsset->baseColorFactor = Vector4(
            pbr.base_color_factor[0],
            pbr.base_color_factor[1],
            pbr.base_color_factor[2],
            pbr.base_color_factor[3]
        );
        materialAsset->metallicFactor = pbr.metallic_factor;
        materialAsset->roughnessFactor = pbr.roughness_factor;

        if (pbr.base_color_texture.texture != nullptr) {
            UUID textureId;
            auto textureIt = modelImportContext.importedTextures.find(pbr.base_color_texture.texture);
            if (textureIt != modelImportContext.importedTextures.end()) {
                textureId = textureIt->second;
            } else {
                throw std::runtime_error("Base color texture has not been imported yet. Ensure that textures are processed before materials.");
            }

            materialAsset->baseColorTexture = textureId;
        }
    }

    materialAsset->emissiveFactor = Vector3(
        material.emissive_factor[0],
        material.emissive_factor[1],
        material.emissive_factor[2]
    );

    return static_cast<const MaterialAsset*>(
        modelImportContext.assetWarehouseService.StoreAsset(
            modelImportContext.sourceAssetMetadata,
            std::move(materialAsset)
        )
    );
}

/**
 * @brief Processes a GLTF mesh and returns a stored MeshAsset pointer.
 * 
 * @param mesh 
 * @param modelImportContext 
 * @return const MeshAsset* 
 */
const MeshAsset* ModelImporter::ProcessMesh(const cgltf_mesh& mesh, const ModelImportContext& modelImportContext) {
    if (modelImportContext.parsedData == nullptr) {
        throw std::runtime_error("Model import context is missing parsed glTF data.");
    }
    if (mesh.primitives_count == 0) {
        throw std::runtime_error("Mesh is missing primitives.");
    }

    // configure the mesh asset with its name and type
    auto meshAsset = std::make_unique<MeshAsset>();
    const std::string meshName = mesh.name != nullptr && mesh.name[0] != '\0'
        ? mesh.name
        : "mesh_" + std::to_string(cgltf_mesh_index(modelImportContext.parsedData, &mesh));
    meshAsset->name = BuildSubAssetNameFromGLTFName(
        modelImportContext.sourceAssetMetadata.path.stem().string(),
        meshName,
        Asset::AssetType::Mesh
    );
    meshAsset->type = Asset::AssetType::Mesh;

    meshAsset->primitives.reserve(static_cast<size_t>(mesh.primitives_count));

    // Process every glTF primitive into an independent vertex/index buffer.  A
    // glTF mesh may contain primitives with different materials or topology, so
    // flattening them into one buffer loses that boundary.
    for (cgltf_size primitiveIndex = 0; primitiveIndex < mesh.primitives_count; ++primitiveIndex) {
        
        const cgltf_primitive& primitive = mesh.primitives[primitiveIndex];
        if (primitive.type != cgltf_primitive_type_triangles
            && primitive.type != cgltf_primitive_type_triangle_strip
            && primitive.type != cgltf_primitive_type_triangle_fan) {
            throw std::runtime_error("Mesh primitive type is not supported unless it is triangle-based.");
        }

        // declare accessors for all 4 vertex attributes
        const cgltf_accessor* positionAccessor = nullptr;
        const cgltf_accessor* normalAccessor = nullptr;
        const cgltf_accessor* tangentAccessor = nullptr;
        const cgltf_accessor* texCoordAccessor = nullptr;

        // assign accessors based on the attribute type and 
        // position accessor is required, while the others are optional
        for (cgltf_size attributeIndex = 0; attributeIndex < primitive.attributes_count; ++attributeIndex) {
            const cgltf_attribute& attribute = primitive.attributes[attributeIndex];
            if (attribute.data == nullptr) {
                continue;
            }

            if (attribute.type == cgltf_attribute_type_position && attribute.index == 0 && positionAccessor == nullptr) {
                positionAccessor = attribute.data;
            } else if (attribute.type == cgltf_attribute_type_normal && attribute.index == 0 && normalAccessor == nullptr) {
                normalAccessor = attribute.data;
            } else if (attribute.type == cgltf_attribute_type_tangent && attribute.index == 0 && tangentAccessor == nullptr) {
                tangentAccessor = attribute.data;
            } else if (attribute.type == cgltf_attribute_type_texcoord && attribute.index == 0 && texCoordAccessor == nullptr) {
                texCoordAccessor = attribute.data;
            }
        }

        // error checking
        // 1. position accessor must exist
        // 2. all accessors must have the same vertex count
        // 3. vertex count must not exceed 32-bit index capacity

        if (positionAccessor == nullptr) {
            throw std::runtime_error("Mesh primitive is missing POSITION data.");
        }
        const cgltf_size vertexCount = positionAccessor->count;
        if (vertexCount == 0) {
            continue;
        }
        if ((normalAccessor != nullptr && normalAccessor->count != vertexCount)
            || (tangentAccessor != nullptr && tangentAccessor->count != vertexCount)
            || (texCoordAccessor != nullptr && texCoordAccessor->count != vertexCount)) {
            throw std::runtime_error("Mesh primitive attributes must have matching vertex counts.");
        }
        if (vertexCount > static_cast<cgltf_size>(std::numeric_limits<uint32_t>::max())) {
            throw std::runtime_error("Mesh vertex count exceeds 32-bit index capacity.");
        }

        PrimitiveData primitiveData;
        if (primitive.material != nullptr) {
            const auto materialIt = modelImportContext.importedMaterials.find(primitive.material);
            if (materialIt == modelImportContext.importedMaterials.end()) {
                throw std::runtime_error("Mesh primitive material has not been imported yet.");
            }
            primitiveData.material = materialIt->second;
        }
        primitiveData.vertices.resize(static_cast<size_t>(vertexCount));

        // Read vertex data from accessors and populate this primitive's buffer.

        for (cgltf_size vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex) {
            
            // initialize vertex and set defaults
            
            Vertex& vertex = primitiveData.vertices[static_cast<size_t>(vertexIndex)];
            vertex.normal = Vector3(0.0f);
            vertex.tangent = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
            vertex.texCoord = Vector2(0.0f);

            // read position

            cgltf_float positionValues[3] = {};
            if (!cgltf_accessor_read_float(positionAccessor, vertexIndex, positionValues, 3)) {
                throw std::runtime_error("Failed to read mesh POSITION attribute data.");
            }
            vertex.position = Vector3(positionValues[0], positionValues[1], positionValues[2]);

            // read normal

            if (normalAccessor != nullptr) {
                cgltf_float normalValues[3] = {};
                if (!cgltf_accessor_read_float(normalAccessor, vertexIndex, normalValues, 3)) {
                    throw std::runtime_error("Failed to read mesh NORMAL attribute data.");
                }
                vertex.normal = Vector3(normalValues[0], normalValues[1], normalValues[2]);
            }

            // read tangent

            if (tangentAccessor != nullptr) {
                cgltf_float tangentValues[4] = {};
                if (!cgltf_accessor_read_float(tangentAccessor, vertexIndex, tangentValues, 4)) {
                    throw std::runtime_error("Failed to read mesh TANGENT attribute data.");
                }
                vertex.tangent = Vector4(
                    tangentValues[0],
                    tangentValues[1],
                    tangentValues[2],
                    tangentValues[3]
                );
            }

            // read texCoord

            if (texCoordAccessor != nullptr) {
                cgltf_float texCoordValues[2] = {};
                if (!cgltf_accessor_read_float(texCoordAccessor, vertexIndex, texCoordValues, 2)) {
                    throw std::runtime_error("Failed to read mesh TEXCOORD_0 attribute data.");
                }
                vertex.texCoord = Vector2(texCoordValues[0], texCoordValues[1]);
            }
        }

        // read index data from the primitive's index accessor

        std::vector<uint32_t> primitiveIndices;
        if (primitive.indices != nullptr) {
            primitiveIndices.reserve(static_cast<size_t>(primitive.indices->count));
            for (cgltf_size index = 0; index < primitive.indices->count; ++index) {
                const cgltf_size rawIndex = cgltf_accessor_read_index(primitive.indices, index);
                if (rawIndex >= vertexCount) {
                    throw std::runtime_error("Mesh primitive index is out of bounds for its vertex buffer.");
                }
                primitiveIndices.push_back(static_cast<uint32_t>(rawIndex));
            }
        } else {
            primitiveIndices.reserve(static_cast<size_t>(vertexCount));
            for (cgltf_size index = 0; index < vertexCount; ++index) {
                primitiveIndices.push_back(static_cast<uint32_t>(index));
            }
        }

        // Populate this primitive's index buffer based on topology
        // 1. triangles: every 3 indices form a triangle, non-overlapping
        // 2. triangle strip: every 3 consecutive indices form a triangle, overlapping
        // 3. triangle fan: first idx center, all others form fan around it

        if (primitive.type == cgltf_primitive_type_triangles) {
            if (primitiveIndices.size() % 3 != 0) {
                throw std::runtime_error("Triangle mesh primitive index count must be divisible by 3.");
            }
            primitiveData.indices.reserve(primitiveIndices.size());
            for (uint32_t primitiveVertexIndex : primitiveIndices) {
                primitiveData.indices.push_back(primitiveVertexIndex);
            }
        } else if (primitive.type == cgltf_primitive_type_triangle_strip) {
            if (primitiveIndices.size() >= 3) {
                primitiveData.indices.reserve((primitiveIndices.size() - 2) * 3);
                for (size_t index = 0; index + 2 < primitiveIndices.size(); ++index) {
                    const uint32_t first = primitiveIndices[index];
                    const uint32_t second = primitiveIndices[index + 1];
                    const uint32_t third = primitiveIndices[index + 2];
                    if (first == second || second == third || first == third) {
                        continue;
                    }

                    if (index % 2 == 0) {
                        primitiveData.indices.push_back(first);
                        primitiveData.indices.push_back(second);
                        primitiveData.indices.push_back(third);
                    } else {
                        primitiveData.indices.push_back(second);
                        primitiveData.indices.push_back(first);
                        primitiveData.indices.push_back(third);
                    }
                }
            }
        } else {
            if (primitiveIndices.size() >= 3) {
                primitiveData.indices.reserve((primitiveIndices.size() - 2) * 3);
                for (size_t index = 1; index + 1 < primitiveIndices.size(); ++index) {
                    const uint32_t first = primitiveIndices[0];
                    const uint32_t second = primitiveIndices[index];
                    const uint32_t third = primitiveIndices[index + 1];
                    if (first == second || second == third || first == third) {
                        continue;
                    }

                    primitiveData.indices.push_back(first);
                    primitiveData.indices.push_back(second);
                    primitiveData.indices.push_back(third);
                }
            }
        }

        meshAsset->primitives.push_back(std::move(primitiveData));
    }

    return static_cast<const MeshAsset*>(
        modelImportContext.assetWarehouseService.StoreAsset(
            modelImportContext.sourceAssetMetadata,
            std::move(meshAsset)
        )
    );
}

/**
 * @brief Recursively processes a node and its children to build the model hierarchy.
 * 
 * @param node CGLTF node to process
 * @param parentModelNode ModelNode struct of parent cglft node
 * @param importData Context containing imported meshes and asset warehouse service.
 * @return std::optional<ModelNode> 
 */
std::optional<ModelNode> ProcessNode(const cgltf_node * node,ModelImportContext& importData) {    
    ModelNode modelNode;
    if (importData.importedMeshes.find(node->mesh) != importData.importedMeshes.end()) {
        modelNode.hasMesh = true;
        modelNode.meshId = importData.importedMeshes[node->mesh];
    }

    modelNode.name = node->name != nullptr ? node->name : "<unnamed>";
    
    cgltf_float matrix[16];
    cgltf_node_transform_local(node, matrix);
    modelNode.relativePosition = Vector3::MatrixToTranslation(matrix);
    modelNode.relativeRotation = Quaternion::MatrixToQuaternion(matrix);
    modelNode.relativeScale = Vector3::MatrixToScale(matrix);

    Logger::Info("ModelImporter", "Processing node with name: " +
        modelNode.name);
    Logger::Info("ModelImporter", "Node relative position: (" +
        std::to_string(modelNode.relativePosition.x) + ", " +
        std::to_string(modelNode.relativePosition.y) + ", " +
        std::to_string(modelNode.relativePosition.z) + ")");
    
    // process children nodes
    for (int i = 0; i < node->children_count; ++i) {
        const cgltf_node * childNode = node->children[i];
        if (std::optional<ModelNode> child = ProcessNode(childNode,importData)) {
            modelNode.children.push_back(std::move(*child));
        }
    }
    return modelNode;
}

/**
 * @brief Compress a model node into a matching hierarchy only consisting of mesh nodes.
 * Builds a mirror of the raw tree, retaining only the nodes that contain meshes.
 * Accumulate transformations from parent nodes to maintain correct positioning of mesh nodes.
 * 
 * @param node 
 * @return std::vector<ModelNode> 
 */
std::vector<ModelNode> CompressNode(ModelNode rawTreeNode, ModelNode * parent) {
    std::vector<ModelNode> compressedNodes;
    ModelNode compressedTreeNode = rawTreeNode;

    // aggregate transform data into compressedTreeNode
    if (parent != nullptr) {
        const glm::quat parentRotation(
            parent->relativeRotation.w,
            parent->relativeRotation.x,
            parent->relativeRotation.y,
            parent->relativeRotation.z
        );
        const glm::vec3 parentScale(
            parent->relativeScale.x,
            parent->relativeScale.y,
            parent->relativeScale.z
        );
        const glm::vec3 childPosition(
            rawTreeNode.relativePosition.x,
            rawTreeNode.relativePosition.y,
            rawTreeNode.relativePosition.z
        );

        // scale and rotate the child position by the parent's transform
        const Vector3 realPosition = parent->relativePosition + (parentRotation * (parentScale * childPosition));

        compressedTreeNode.relativePosition = realPosition;
        compressedTreeNode.relativeRotation = parent->relativeRotation * rawTreeNode.relativeRotation;
        compressedTreeNode.relativeScale = parent->relativeScale * rawTreeNode.relativeScale;
    }

    // case 1: current node has a mesh, retain it and compress its children relative to this node

    if (rawTreeNode.hasMesh) {
        // retain mesh nodes and keep descendants relative to this one
        // rebuild children array with the compressed children
        Logger::Info("ModelImporter", "Compressing node with mesh: " + rawTreeNode.name);
        compressedTreeNode.children.clear();
        for (auto& child : rawTreeNode.children) {
            std::vector<ModelNode> compressedChildren = CompressNode(std::move(child), nullptr);
            compressedTreeNode.children.insert(
                compressedTreeNode.children.end(),
                std::make_move_iterator(compressedChildren.begin()),
                std::make_move_iterator(compressedChildren.end())
            );
        }
        // push final compressed node to our list of compressed nodes
        compressedNodes.push_back(std::move(compressedTreeNode));
        return compressedNodes;
    }

    // case 2: current node doesn't have mesh, 

    for (auto& child : rawTreeNode.children) {
        std::vector<ModelNode> childNodes = CompressNode(std::move(child), &compressedTreeNode);
        for (auto& childNode : childNodes) {
            compressedNodes.push_back(std::move(childNode));
        }
    }

    return compressedNodes;
}

const std::unique_ptr<ModelAsset> ModelImporter::CompileModelAsset(const cgltf_scene& scene, ModelImportContext& importData) {
    std::unique_ptr<ModelAsset> modelAsset = std::make_unique<ModelAsset>();
    
    modelAsset->name = importData.sourceAssetMetadata.path.stem().string();
    modelAsset->type = Asset::AssetType::Model;
    modelAsset->meshes.reserve(importData.importedMeshes.size());
    for (const auto& [mesh, meshId] : importData.importedMeshes) {
        modelAsset->meshes.push_back(meshId);
    }

    // traverse the entire scene tree and build the model hierarchy

    std::vector<ModelNode> rawTrees;
    for (int i = 0; i < scene.nodes_count; ++i) {
        const cgltf_node * node = scene.nodes[i];
        if (std::optional<ModelNode> modelNode = ProcessNode(node, importData)) {
            rawTrees.push_back(std::move(*modelNode));
        }
    }

    // compress the model hierarchy to be meshes only

    std::vector<ModelNode> compressedTrees;
    for (auto& rawTree : rawTrees) {
        auto compressed = CompressNode(rawTree, nullptr);
        for (auto& node : compressed) {
            compressedTrees.push_back(std::move(node));
        }
    }

    modelAsset->trees = std::move(compressedTrees);
    return modelAsset;
}
