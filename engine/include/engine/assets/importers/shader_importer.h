#pragma once

#include "engine/assets/asset_data.h"
#include "engine/assets/asset_structures.h"

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
