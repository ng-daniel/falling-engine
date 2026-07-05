#ifndef ENGINE_ASSETS_TEXTURE_IMPORTER_H
#define ENGINE_ASSETS_TEXTURE_IMPORTER_H

#include <vector>
#include "asset_structures.h"

/**
 * @brief Handles loading of texture assets.
 */
class TextureImporter : public AssetImporter {
public:
    TextureImporter() = default;
    ~TextureImporter() override = default;

    std::unique_ptr<Asset> LoadAsset(const std::filesystem::path& path) override;
    std::string_view GetName() override { return importerName; }
    std::string_view GetType() override { return importerType; }
private:
    std::string_view importerName = "TextureImporter"; // should be same as class name
    std::string_view importerType = "Texture"; // should be same as enum in asset_structures
};

#endif // ENGINE_ASSETS_TEXTURE_IMPORTER_H