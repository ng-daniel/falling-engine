#ifndef ENGINE_ASSETS_ASSET_STRUCTURES_H
#define ENGINE_ASSETS_ASSET_STRUCTURES_H

#include <filesystem>
#include <string>

#include "engine/utils/uuid.h"

#include "asset_data.h"

struct AssetMetadata {
    UUID id;
    std::string type;
    std::string importer;
    std::filesystem::path path;
    bool loaded; // in CPU memory or not
};

/**
 * @brief Abstract class for asset loaders, should not be used by itself.
 */
class AssetImporter {
public:
    AssetImporter() = default;
    ~AssetImporter() = default;

    // is a vector of assets because some importers may generate multiple assets
    static std::vector<std::unique_ptr<Asset>> LoadAsset(AssetMetadata& metadata);
    static std::string_view GetName();
    static std::string_view GetType();
};

#endif // ENGINE_ASSETS_ASSET_STRUCTURES_H