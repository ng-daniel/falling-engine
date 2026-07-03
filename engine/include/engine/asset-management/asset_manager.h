#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <iostream>
#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <vector>

#include "asset_structures.h"

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