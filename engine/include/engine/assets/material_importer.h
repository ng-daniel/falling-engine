#ifndef ENGINE_ASSETS_MATERIAL_IMPORTER_H
#define ENGINE_ASSETS_MATERIAL_IMPORTER_H

#include <vector>
#include "asset_structures.h"

/**
 * @brief Handles loading of material assets.
 */
class MaterialImporter : public AssetImporter {
public:
    ~MaterialImporter() override = default;

    std::unique_ptr<Asset> LoadAsset(const std::filesystem::path& path) override;
    std::string_view GetName() override { return importerName; }
    std::string_view GetType() override { return importerType; }

private:
    std::string_view importerName = "MaterialImporter"; // should be same as class name
    std::string_view importerType = "Material"; // should be same as enum in asset_structures
};

#endif // ENGINE_ASSETS_MATERIAL_IMPORTER_H