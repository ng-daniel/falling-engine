#include "engine/assets/asset_data.h"
#include <vector>
#include "engine/assets/model_importer.h"

/**
 * @brief Loads a GLTF model asset from the specified path.
 * 
 * @param path The file path to the model asset.
 * @return A vector of unique pointers to Asset objects representing the loaded model.
 */
std::vector<std::unique_ptr<Asset>>
ModelImporter::LoadAsset(AssetMetadata& metadata) {
    
    std::vector<std::unique_ptr<Asset>> importedAssets;
    std::unordered_map<cgltf_texture*, UUID> importedTextures;
    std::unordered_map<cgltf_material*, UUID> importedMaterials;
    std::unordered_map<cgltf_mesh*, UUID> importedMeshes;

    cgltf_file_type type = metadata.path.extension() == ".gltf" ? cgltf_file_type_gltf : cgltf_file_type_glb;

    cgltf_options options = {type};
    cgltf_data * data = NULL;
    cgltf_result result = cgltf_parse_file(&options, metadata.path.string().c_str(), &data);
    if (result == cgltf_result_success)
    {
        // process all textures in the model
        for (cgltf_size i = 0; i < data->textures_count; ++i)
        {
            cgltf_texture * texture = &data->textures[i];
            importedAssets.push_back(ProcessTexture(*texture));
        }
        
        // process all meshes in the model
        for (cgltf_size i = 0; i < data->meshes_count; ++i)
        {
            cgltf_mesh * mesh = &data->meshes[i];
            importedAssets.push_back(ProcessMesh(*mesh));
        }

        // process all materials in the model
        for (cgltf_size i = 0; i < data->materials_count; ++i)
        {
            cgltf_material * material = &data->materials[i];
            importedAssets.push_back(ProcessMaterial(*material));
        }

        cgltf_free(data);
    }

    return importedAssets;
}

std::unique_ptr<MeshAsset> ModelImporter::ProcessMesh(const cgltf_mesh& mesh) {
    return nullptr;
}

std::unique_ptr<MaterialAsset> ModelImporter::ProcessMaterial(const cgltf_material& material) {
    return nullptr;
}

std::unique_ptr<TextureAsset> ModelImporter::ProcessTexture(const cgltf_texture& texture) {
    return nullptr;
}