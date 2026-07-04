#ifndef ENGINE_ASSETS_ASSET_STRUCTURES_H
#define ENGINE_ASSETS_ASSET_STRUCTURES_H

#include <cstdint>
#include <filesystem>
#include <string>

#include "engine/utils/uuid.h"

enum AssetType {
    Model,
    Mesh,
    Material,
    Texture,
    Shader
};

struct AssetMetadata {
    UUID id;
    std::string type;
    std::string importer;
    std::filesystem::path path;
    bool loaded; // in CPU memory or not
};

/**
 * @brief Struct for assets, should not be used by itself.
 * But also can't be abstract because it is returned by AssetLoaders.
 */
struct Asset {
    UUID id;
    std::string name;
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