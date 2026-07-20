#ifndef ENGINE_ASSETS_IMAGE_IMPORTER_H
#define ENGINE_ASSETS_IMAGE_IMPORTER_H

#include "engine/assets/asset_data.h"
#include <vector>

/**
 * @brief Handles loading of image assets.
 */
class ImageImporter {
public:
    ~ImageImporter() = default;

    static std::vector<std::unique_ptr<Asset>> LoadAsset(SourceAssetMetadata& metadata);
    static std::vector<std::unique_ptr<Asset>> LoadAssetFromMemory(const std::vector<unsigned char>& data);
    static std::vector<unsigned char> DecodeDataUri(const char * uri);
    static std::string_view GetName() { return importerName; }
    static std::string_view GetType() { return importerType; }
private:
    constexpr static const std::string_view importerName = "ImageImporter"; // should be same as class name
    constexpr static const std::string_view importerType = "Image"; // should be same as enum in asset_structures
};

#endif // ENGINE_ASSETS_IMAGE_IMPORTER_H