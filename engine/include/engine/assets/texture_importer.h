#ifndef ENGINE_ASSETS_TEXTURE_IMPORTER_H
#define ENGINE_ASSETS_TEXTURE_IMPORTER_H

#include "asset_data.h"
#include <vector>

/**
 * @brief Handles loading of texture assets.
 */
class TextureImporter {
public:
    ~TextureImporter() = default;

    static std::vector<std::unique_ptr<Asset>> LoadAsset(AssetMetadata& metadata);
    static std::vector<std::unique_ptr<Asset>> LoadAssetFromMemory(const std::vector<unsigned char>& data);
    static std::string_view GetName() { return importerName; }
    static std::string_view GetType() { return importerType; }
private:
    constexpr static const std::string_view importerName = "TextureImporter"; // should be same as class name
    constexpr static const std::string_view importerType = "Texture"; // should be same as enum in asset_structures
};

#endif // ENGINE_ASSETS_TEXTURE_IMPORTER_H