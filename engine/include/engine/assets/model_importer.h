#ifndef ENGINE_ASSETS_MODEL_IMPORTER_H
#define ENGINE_ASSETS_MODEL_IMPORTER_H

#include "asset_structures.h"

/**
 * @brief Handles loading of glTF model assets.
 */
class ModelImporter : public AssetImporter {
public:
    ~ModelImporter() override = default;

    std::unique_ptr<Asset> LoadAsset(const std::filesystem::path& path) override;
    std::string_view GetName() override { return importerName; }
    std::string_view GetType() override { return importerType; }
private:
    std::string_view importerName = "ModelImporter"; // should be same as class name
    std::string_view importerType = "Model"; // should be same as enum in asset_structures
};

#endif // ENGINE_ASSETS_MODEL_IMPORTER_H