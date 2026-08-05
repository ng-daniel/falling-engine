#ifndef ENGINE_ASSETS_SHADER_IMPORTER_H
#define ENGINE_ASSETS_SHADER_IMPORTER_H

#include "engine/assets/asset_data.h"

class AssetWarehouseService;

/**
 * @brief Handles loading of shader assets.
 */
class ShaderImporter {
public:
    ~ShaderImporter() = default;

    static const ShaderAsset* LoadAsset(SourceAssetMetadata& metadata, AssetWarehouseService& assetWarehouseService);
    static std::string_view GetName() { return importerName; }
    static std::string_view GetType() { return importerType; }
private:
    constexpr static const std::string_view importerName = "ShaderImporter";
    constexpr static const std::string_view importerType = "Shader";
};

#endif // ENGINE_ASSETS_SHADER_IMPORTER_H