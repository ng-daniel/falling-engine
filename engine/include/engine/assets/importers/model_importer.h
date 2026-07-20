#ifndef ENGINE_ASSETS_MODEL_IMPORTER_H
#define ENGINE_ASSETS_MODEL_IMPORTER_H

#include "engine/assets/asset_data.h"

#include "cgltf.h"
#include "engine/assets/asset_warehouse_service.h"

/**
* @brief Represents a task for importing a model asset.
* Stores temporary mapping buffers
* ONLY USE INTERNALLY by the ModelImporter class,
* since cgltf data is only valid during the import process
*
* Also contains a reference to AssetWarehouseService to allow for metadata lookups,
* primarily for external image assets for .glTF separate files format.
*/
class ModelImportContext {
public:
    ModelImportContext(AssetWarehouseService& assetWarehouseService, const std::filesystem::path& modelDirectory) 
        : assetWarehouseService(assetWarehouseService), modelDirectory(modelDirectory) {}
    ~ModelImportContext() = default;

    AssetWarehouseService& assetWarehouseService;
    std::filesystem::path modelDirectory;

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

    static std::vector<std::unique_ptr<Asset>> LoadAsset(
        SourceAssetMetadata& metadata,
        AssetWarehouseService& assetWarehouseService
    );
    static std::string_view GetName() { return importerName; }
    static std::string_view GetType() { return importerType; }
private:
    constexpr static const std::string_view importerName = "ModelImporter";
    constexpr static const std::string_view importerType = "Model";

    static std::unique_ptr<MeshAsset> ProcessMesh(const cgltf_mesh& mesh, ModelImportContext& importData);
    static std::unique_ptr<MaterialAsset> ProcessMaterial(const cgltf_material& material, ModelImportContext& importData);
    static std::unique_ptr<TextureAsset> ProcessTexture(const cgltf_texture& texture, ModelImportContext& importData);
    static std::unique_ptr<ImageAsset> ProcessImage(const cgltf_image& image, ModelImportContext& importData);
};

#endif // ENGINE_ASSETS_MODEL_IMPORTER_H