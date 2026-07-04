#ifndef TEXTURE_IMPORTER_H
#define TEXTURE_IMPORTER_H

#include <vector>
#include "asset_structures.h"

struct TextureAsset : public Asset {
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

    std::unique_ptr<Asset> LoadAsset(const std::filesystem::path& path) override;
    std::string_view GetName() override { return importerName; }
private:
    std::string_view importerName = "TextureImporter"; // should be same as class name
};

#endif // TEXTURE_IMPORTER_H