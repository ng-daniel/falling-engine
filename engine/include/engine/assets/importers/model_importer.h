#ifndef ENGINE_ASSETS_MODEL_IMPORTER_H
#define ENGINE_ASSETS_MODEL_IMPORTER_H

#include "engine/assets/asset_data.h"

#include "cgltf.h"

/**
* @brief Represents a task for importing a model asset.
* Stores temporary mapping buffers
* ONLY USE INTERNALLY by the ModelImporter class,
* since cgltf data is only valid during the import process
*/
class ModelImportData {
public:
    ModelImportData() = default;
    ~ModelImportData() = default;

    // cgltf to engine asset mappings in order of lowest to highest dependency level
    std::unordered_map<cgltf_image*, UUID> importedImages;
    std::unordered_map<cgltf_texture*, UUID> importedTextures;
    std::unordered_map<cgltf_material*, UUID> importedMaterials;
    std::unordered_map<cgltf_mesh*, UUID> importedMeshes;
};

/**
 * @brief Handles loading of model assets.
 */
class ModelImporter {
public:
    ~ModelImporter() = default;

    static std::vector<std::unique_ptr<Asset>> LoadAsset(SourceAssetMetadata& metadata);
    static std::string_view GetName() { return importerName; }
    static std::string_view GetType() { return importerType; }
private:
    constexpr static const std::string_view importerName = "ModelImporter";
    constexpr static const std::string_view importerType = "Model";

    static std::unique_ptr<MeshAsset> ProcessMesh(const cgltf_mesh& mesh, ModelImportData& importData);
    static std::unique_ptr<MaterialAsset> ProcessMaterial(const cgltf_material& material, ModelImportData& importData);
    static std::unique_ptr<TextureAsset> ProcessTexture(const cgltf_texture& texture, ModelImportData& importData);
    static std::unique_ptr<ImageAsset> ProcessImage(const cgltf_image& image, ModelImportData& importData);
};

#endif // ENGINE_ASSETS_MODEL_IMPORTER_H