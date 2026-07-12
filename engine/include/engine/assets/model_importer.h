#ifndef ENGINE_ASSETS_MODEL_IMPORTER_H
#define ENGINE_ASSETS_MODEL_IMPORTER_H

#include "asset_structures.h"
#include "asset_data.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

/**
 * @brief Handles loading of model assets.
 */
class ModelImporter {
public:
    ~ModelImporter() = default;

    static std::vector<std::unique_ptr<Asset>> LoadAsset(AssetMetadata& metadata);
    static std::string_view GetName() { return importerName; }
    static std::string_view GetType() { return importerType; }
private:
    constexpr static const std::string_view importerName = "ModelImporter";
    constexpr static const std::string_view importerType = "Model";

    static std::unique_ptr<MeshAsset> ProcessMesh(const cgltf_mesh& mesh);
    static std::unique_ptr<MaterialAsset> ProcessMaterial(const cgltf_material& material);
    static std::unique_ptr<TextureAsset> ProcessTexture(const cgltf_texture& texture);
};

#endif // ENGINE_ASSETS_MODEL_IMPORTER_H