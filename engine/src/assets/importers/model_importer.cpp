#include "engine/assets/asset_data.h"
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "engine/assets/importers/model_importer.h"
#include "engine/assets/importers/image_importer.h"


/**
 * @brief Loads a GLTF model asset from the specified path.
 * 
 * @param path The file path to the model asset.
 * @return A vector of unique pointers to Asset objects representing the loaded model.
 */
std::vector<std::unique_ptr<Asset>>
ModelImporter::LoadAsset(SourceAssetMetadata& metadata) {
    
    ModelImportData tempData;
    std::vector<std::unique_ptr<Asset>> importedAssets;

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
        std::unique_ptr<TextureAsset> textureAsset = ProcessTexture(*texture, tempData);
        tempData.importedTextures[texture] = textureAsset->id;
        importedAssets.push_back(std::move(textureAsset));
    }
    
    // process all materials in the model

    for (cgltf_size i = 0; i < data->materials_count; ++i)
    {
        cgltf_material * material = &data->materials[i];
        std::unique_ptr<MaterialAsset> materialAsset = ProcessMaterial(*material, tempData);
        tempData.importedMaterials[material] = materialAsset->id;
        importedAssets.push_back(std::move(materialAsset));
    }

    // process all meshes in the model

    for (cgltf_size i = 0; i < data->meshes_count; ++i)
    {
        cgltf_mesh * mesh = &data->meshes[i];
        std::unique_ptr<MeshAsset> meshAsset = ProcessMesh(*mesh, tempData);
        tempData.importedMeshes[mesh] = meshAsset->id;
        importedAssets.push_back(std::move(meshAsset));
    }

    cgltf_free(data);

    return importedAssets;
}

std::unique_ptr<ImageAsset> ModelImporter::ProcessImage(
    const cgltf_image& image,
    ModelImportData& importData) {
    
    if (!image.uri && !image.buffer_view) {
        throw std::runtime_error("CGLTF Image has no URI or buffer view");
    }

    if (image.uri) {
        
    }

    return nullptr;
}

// std::unique_ptr<TextureAsset> ModelImporter::ProcessTexture(const cgltf_texture& texture) {
//     cgltf_image * image = texture.image;
//     if (!image->uri) {
//         throw std::runtime_error("Texture image URI is null");
//     }

//     std::unique_ptr<TextureAsset> textureAsset;
//     // cgltf_accessor * accessor = 
// }

std::unique_ptr<MaterialAsset> ModelImporter::ProcessMaterial(const cgltf_material& material, ModelImportData& importData) {
    return nullptr;
}

std::unique_ptr<MeshAsset> ModelImporter::ProcessMesh(const cgltf_mesh& mesh, ModelImportData& importData) {
    return nullptr;
}
