#ifndef ENGINE_ASSETS_ASSET_MANAGER_H
#define ENGINE_ASSETS_ASSET_MANAGER_H

#include <iostream>
#include <filesystem>
#include <memory>
#include <unordered_map>

#include "engine/serialization/asset_metadata_serializer.h"
#include "engine/serialization/asset_uuid_serializer.h"

#include "asset_structures.h"
#include "asset_helpers.h"


/**
 * @brief Manages all loading, storage, and distribution of assets for the game.
 */
class AssetManager {
public:
    AssetManager(std::filesystem::path root);
    ~AssetManager();

    /**
     * @brief Requests an asset of the specified type and ID.
     * 
     * @tparam T The type of the asset to request.
     * @param id The ID of the asset to request.
     * 
     * @return A pointer to the requested asset, or nullptr if not found.
     * 
     * @details
     * This is the main interface for external code to request assets from the AssetManager.
     * First searches the loadedAssets map for the asset.
     * If not found, it loads the asset from disk by looking it up
     * in the assetMetadatas map and adds it to the loadedAssets map.
     */
    template <typename T>
    T* RequestAsset(UUID id) {
        
        // pull metadata
        auto metadataIterator = assetMetadatas.find(id);
        if (metadataIterator == assetMetadatas.end()) {
            std::cout << "Asset with ID " + std::to_string(id) + " not found in assetMetadatas." << std::endl;
            return nullptr;
        }
        AssetMetadata& metadata = metadataIterator->second;

        // load asset if not already loaded
        if (loadedAssets.find(id) == loadedAssets.end()) {            
            ImportSourceAsset(metadata);
        }
        
        // validate and return asset ptr
        std::unique_ptr<Asset>& assetPtr = loadedAssets.find(id)->second;
        if (!assetPtr) {
            std::cout << "Asset with ID " + std::to_string(id) + " failed to load." << std::endl;
            return nullptr;
        }
        T* typedAssetPtr = static_cast<T*>(assetPtr.get());
        if (!typedAssetPtr || 
            typedAssetPtr->type != GetAssetTypeFromString(metadata.type)
        ) {
            std::cout << "Asset with ID " + std::to_string(id) + " is not of the requested type." << std::endl;
            return nullptr;
        }

        std::cout << "Asset with ID " + std::to_string(id) + " loaded and returned." << std::endl;
        return typedAssetPtr;
    }

    /**
     * @brief Requests an asset of the specified type and ID, but returns a const pointer.
     * @tparam T The type of the asset to request.
     * @param id The ID of the asset to request.
     * @return A const pointer to the requested asset, or nullptr if not found.
     */
    template <typename T>
    const T* RequestAssetReadOnly(UUID id) {
        return RequestAsset<T>(id);
    }

private:
    const std::string ASSET_UUID_EXTENSION = ".fid"; // stands for falling id
    const std::string ASSET_METADATA_EXTENSION = ".fmeta"; // stands for falling metadata

    std::filesystem::path rootDirectory;

    // metadata serializer
    AssetMetadataSerializer metadataSerializer;
    AssetUUIDSerializer uuidSerializer;

    // data storage
    std::unordered_map<UUID, AssetMetadata> assetMetadatas;
    std::unordered_map<UUID, std::unique_ptr<Asset>> loadedAssets;

    // mappings
    std::unordered_map<std::string, std::reference_wrapper<AssetImporter>> extensionToImporter;

    void ProcessAssetDirectory(const std::filesystem::path& assetDirectory);

    void ImportSourceAsset(AssetMetadata& metadata);
    void RegisterLoadedAsset(AssetMetadata& metadata, std::unique_ptr<Asset> asset);
    
    AssetMetadata GenerateMetadata(const std::filesystem::path& assetPath);
    AssetMetadata ReadMetadataAndUUID(const std::filesystem::path& metadataFilePath);
    void ValidateMetadataAndUUID(AssetMetadata& metadata, const std::filesystem::path& metadataFilePath);
    void WriteMetadataAndUUID(const AssetMetadata& metadata, const std::filesystem::path& assetPath);
    std::filesystem::path GenerateMetadataFilePath(const std::filesystem::path& assetPath);
    std::filesystem::path GenerateUUIDFilePath(const std::filesystem::path& assetPath);
};

#endif // ENGINE_ASSETS_ASSET_MANAGER_H