#ifndef ENGINE_ASSETS_SHADER_IMPORTER_H
#define ENGINE_ASSETS_SHADER_IMPORTER_H

#include "asset_structures.h"

/**
 * @brief Handles loading of shader assets.
 */
class ShaderImporter : public AssetImporter {
public:
    ~ShaderImporter() override = default;

    std::vector<std::unique_ptr<Asset>> LoadAsset(const std::filesystem::path& path) override;
    std::string_view GetName() override { return importerName; }
    std::string_view GetType() override { return importerType; }
private:
    std::string_view importerName = "ShaderImporter"; // should be same as class name
    std::string_view importerType = "Shader"; // should be same as enum in asset_structures
};

#endif // ENGINE_ASSETS_SHADER_IMPORTER_H