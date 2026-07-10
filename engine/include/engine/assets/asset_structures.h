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
    virtual ~AssetImporter() = default;

    virtual std::unique_ptr<Asset> LoadAsset(const std::filesystem::path& path) = 0;
    virtual std::string_view GetName() = 0;
    virtual std::string_view GetType() = 0;
};

#endif // ENGINE_ASSETS_ASSET_STRUCTURES_H