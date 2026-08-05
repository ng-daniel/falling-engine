#ifndef ENGINE_ASSETS_IMAGE_IMPORTER_H
#define ENGINE_ASSETS_IMAGE_IMPORTER_H

#include "engine/assets/asset_data.h"
#include <vector>

class AssetWarehouseService;

/**
 * @brief Handles loading of image assets.
 */
class ImageImporter {
public:
    ~ImageImporter() = default;

    static const ImageAsset* LoadAsset(SourceAssetMetadata& metadata, AssetWarehouseService& assetWarehouseService);
    static const ImageAsset* LoadAssetFromMemory(
        SourceAssetMetadata& metadata,
        AssetWarehouseService& assetWarehouseService,
        const std::vector<unsigned char>& data,
        const std::string& subAssetIdentifier,
        RuntimeAssetMetadata* runtimeMetadata = nullptr
    );
    static std::vector<unsigned char> DecodeDataUri(const char * uri);
    static std::string_view GetName() { return importerName; }
    static std::string_view GetType() { return importerType; }
private:
    constexpr static const std::string_view importerName = "ImageImporter"; // should be same as class name
    constexpr static const std::string_view importerType = "Image"; // should be same as enum in asset_structures
};

#endif // ENGINE_ASSETS_IMAGE_IMPORTER_H