#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <iostream>
#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <vector>

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
 * @brief Abstract class for assets, should not be used by itself.
 */
class Asset {
public:
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

/**
 * @brief Manages all loading, storage, and distribution of assets for the game.
 */
class AssetManager {
public:
    AssetManager();
    ~AssetManager();

    /**
     * @brief Requests an asset of the specified type and ID.
     * @tparam T The type of the asset to request.
     * @param id The ID of the asset to request.
     * @return A reference to the requested asset.
     */
    template<typename T>
    T& RequestAsset(AssetID id);

private:
    const std::string ASSET_METADATA_EXTENSION = ".fmeta"; // stands for falling metadata
    
    // data storage
    std::unordered_map<AssetID, AssetMetadata> assetMetadatas;
    std::unordered_map<AssetID, Asset> loadedAssets;

    // mappings
    std::unordered_map<std::string, AssetImporter> extensionToImporter;  

    void ProcessAssetDirectory(const std::filesystem::path& assetDirectory);

    std::vector<Asset> 
    ImportSourceAsset(AssetMetadata& metadata);
    
    AssetImporter& GetImporterForExtension(const std::string& extension);
    
    AssetMetadata GenerateMetadata(const std::filesystem::path& assetPath);
    AssetMetadata ParseMetadata(const std::filesystem::path& metadataFilePath);

    AssetID GenerateSourceAssetID();
    AssetID GenerateSubAssetID(AssetID parentID, const std::string& subAssetName);
};

#endif // ASSET_MANAGER_H