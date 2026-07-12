#ifndef ENGINE_ASSETS_SHADER_IMPORTER_H
#define ENGINE_ASSETS_SHADER_IMPORTER_H

#include "asset_structures.h"

/**
 * @brief Handles loading of shader assets.
 */
class ShaderImporter : public AssetImporter {
public:
    ~ShaderImporter() = default;

    static std::vector<std::unique_ptr<Asset>> LoadAsset(AssetMetadata& metadata);
    static std::string_view GetName() { return importerName; }
    static std::string_view GetType() { return importerType; }
private:
    constexpr static const std::string_view importerName = "ShaderImporter";
    constexpr static const std::string_view importerType = "Shader";
};

#endif // ENGINE_ASSETS_SHADER_IMPORTER_H