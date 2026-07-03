#ifndef TEXTURE_IMPORTER_H
#define TEXTURE_IMPORTER_H

#include <vector>
#include "asset_structures.h"

class TextureAsset : public Asset {
public:
    TextureAsset() = default;
    ~TextureAsset() override = default;

    // texture properties
    int width;
    int height;
    int numChannels;
    std::vector<unsigned char> data; // raw pixel data
};

/**
 * @brief Handles loading of texture assets.
 */
class TextureImporter : public AssetImporter {
public:
    TextureImporter() = default;
    ~TextureImporter() override = default;

    Asset& LoadAsset(const std::filesystem::path& path) override;
};

#endif // TEXTURE_IMPORTER_H