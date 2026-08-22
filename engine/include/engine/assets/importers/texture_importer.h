#pragma once

#include "engine/assets/asset_data.h"
#include "engine/assets/asset_structures.h"

class AssetWarehouseService;

/**
 * @brief Handles loading of texture assets.
 */
class TextureImporter {
public:
    ~TextureImporter() = default;

    static const TextureAsset* LoadAsset(SourceAssetMetadata& metadata, AssetWarehouseService& assetWarehouseService);
    static const TextureAsset* LoadAssetFromImage(
        SourceAssetMetadata& metadata,
        AssetWarehouseService& assetWarehouseService,
        UUID imageId,
        const std::string& subAssetIdentifier = ""
    );
    static std::string_view GetName() { return importerName; }
    static std::string_view GetType() { return importerType; }
private:
    constexpr static const std::string_view importerName = "TextureImporter"; // should be same as class name
    constexpr static const std::string_view importerType = "Texture"; // should be same as enum in asset_structures
};
