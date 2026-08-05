#ifndef ENGINE_ASSETS_TEXTURE_IMPORTER_H
#define ENGINE_ASSETS_TEXTURE_IMPORTER_H

#include "engine/assets/asset_data.h"
#include <vector>

class AssetWarehouseService;

/**
 * @brief Handles loading of texture assets.
 */
class TextureImporter {
public:
    ~TextureImporter() = default;

    static const TextureAsset* ImportTexture(SourceAssetMetadata& metadata, AssetWarehouseService& assetWarehouseService);
    static const TextureAsset* BuildDefaultTextureForImage(
        SourceAssetMetadata& metadata,
        AssetWarehouseService& assetWarehouseService,
        UUID imageId
    );
    static std::string_view GetName() { return importerName; }
    static std::string_view GetType() { return importerType; }
private:
    constexpr static const std::string_view importerName = "TextureImporter"; // should be same as class name
    constexpr static const std::string_view importerType = "Texture"; // should be same as enum in asset_structures
};

#endif // ENGINE_ASSETS_TEXTURE_IMPORTER_H