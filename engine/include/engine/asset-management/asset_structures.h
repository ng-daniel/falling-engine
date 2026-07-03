
#ifndef ASSET_STRUCTURES_H
#define ASSET_STRUCTURES_H

#include <cstdint>
#include <filesystem>
#include <string>

using AssetID = std::uint32_t;

enum AssetType {
    Model,
    Mesh,
    Material,
    Texture,
    Shader
};

struct AssetMetadata {
    AssetID id;
    std::string type;
    std::string importer;
    std::filesystem::path path;
    bool loaded; // in CPU memory or not
};

/**
 * @brief Struct for assets, should not be used by itself.
 */
struct Asset {
    Asset();
    virtual ~Asset() = 0;
    AssetID id;
    std::string name;
};

/**
 * @brief Abstract class for asset loaders, should not be used by itself.
 */
class AssetImporter {
public:
    AssetImporter() = default;
    virtual ~AssetImporter() = 0;

    virtual Asset& LoadAsset(const std::filesystem::path& path) = 0;
    std::string GetImporterName() const { return importerName; }

private:
    std::string importerName;
};

#endif // ASSET_STRUCTURES_H