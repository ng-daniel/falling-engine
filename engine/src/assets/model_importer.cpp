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
ModelImporter::LoadAsset(const std::filesystem::path& path) {
    
    std::vector<std::unique_ptr<MeshAsset>> importedMeshes;
    std::vector<std::unique_ptr<MaterialAsset> > importedMaterials;
    std::vector<std::unique_ptr<TextureAsset> > importedTextures;

    cgltf_file_type type = path.extension() == ".gltf" ? cgltf_file_type_gltf : cgltf_file_type_glb;

    cgltf_options options = {type};
    cgltf_data * data = NULL;
    cgltf_result result = cgltf_parse_file(&options, "scene.gltf", &data);
    if (result == cgltf_result_success)
    {
        // process all textures in the model
        for (cgltf_size i = 0; i < data->textures_count; ++i)
        {
            cgltf_texture * texture = &data->textures[i];
            importedTextures.push_back(ProcessTexture(*texture));
        }
        
        // process all meshes in the model
        for (cgltf_size i = 0; i < data->meshes_count; ++i)
        {
            cgltf_mesh * mesh = &data->meshes[i];
            importedMeshes.push_back(ProcessMesh(*mesh));
        }

        // process all materials in the model
        for (cgltf_size i = 0; i < data->materials_count; ++i)
        {
            cgltf_material * material = &data->materials[i];
            importedMaterials.push_back(ProcessMaterial(*material));
        }

        cgltf_free(data);
    }

    std::vector<std::unique_ptr<Asset>> importedAssets;
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