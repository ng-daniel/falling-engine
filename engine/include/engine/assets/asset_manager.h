#ifndef ENGINE_ASSETS_ASSET_MANAGER_H
#define ENGINE_ASSETS_ASSET_MANAGER_H

#include <iostream>
#include <filesystem>

#include "engine/assets/warehouse/asset_warehouse_service.h"

#include "asset_helpers.h"


/**
 * @brief Manages asset imports and serves asset requests through the warehouse.
 */
class AssetManager {
public:
    AssetManager(std::filesystem::path root);
    ~AssetManager() = default;

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
     * First searches the asset warehouse for the asset.
     * If not found, it loads the asset from disk by looking up metadata
     * in the asset warehouse and storing the loaded asset there.
     */
    template <typename T>
    T* RequestAsset(UUID id) {
        SourceAssetMetadata* metadata = assetWarehouseService.FindMetadata(id);
        if (!metadata) {
            std::cout << "Asset with ID " + std::to_string(id) + " not found in asset warehouse metadata." << std::endl;
            return nullptr;
        }

        if (!assetWarehouseService.HasLoadedAsset(id)) {
            ImportSourceAsset(*metadata);
        }

        Asset* asset = assetWarehouseService.GetLoadedAsset(id);
        if (!asset) {
            std::cout << "Asset with ID " + std::to_string(id) + " failed to load." << std::endl;
            return nullptr;
        }

        T* typedAssetPtr = static_cast<T*>(asset);
        if (!typedAssetPtr || 
            typedAssetPtr->type != GetAssetTypeFromString(metadata->type)
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
    AssetWarehouseService assetWarehouseService;
    void ImportSourceAsset(SourceAssetMetadata& metadata);
};

#endif // ENGINE_ASSETS_ASSET_MANAGER_H