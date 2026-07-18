#ifndef ENGINE_ASSETS_ASSET_MANAGER_H
#define ENGINE_ASSETS_ASSET_MANAGER_H

#include <iostream>
#include <filesystem>

#include "engine/assets/asset_handle.h"
#include "engine/assets/asset_warehouse_service.h"
#include "engine/assets/asset_importer_service.h"

#include "engine/assets/asset_helpers.h"


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
     * @param id The runtime asset ID of the asset to request.
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
        SourceAssetMetadata* metadata = assetWarehouseService.FindSourceMetadata(id);
        if (!metadata) {
            std::cout << "Asset with ID " + std::to_string(id) + " not found in asset warehouse metadata." << std::endl;
            return nullptr;
        }

        // import and store in warehouse if not loaded
        if (!assetWarehouseService.HasLoadedAsset(id)) {
            std::vector<std::unique_ptr<Asset>> assets = assetImporterService.ImportSourceAsset(*metadata);
            for (auto& asset : assets) {
                assetWarehouseService.StoreLoadedAsset(*metadata, std::move(asset));
            }
        }

        // retrieval and checks
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
    template <typename T>
    T* RequestAsset(AssetHandle id) {
        return RequestAsset<T>(id.GetUUID());
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
    template <typename T>
    const T* RequestAssetReadOnly(AssetHandle id) {
        return RequestAssetReadOnly<T>(id.GetUUID());
    }

    AssetWarehouseService& GetAssetWarehouseService() {
        return assetWarehouseService;
    }

private:
    AssetImporterService assetImporterService;
    AssetWarehouseService assetWarehouseService;
};

#endif // ENGINE_ASSETS_ASSET_MANAGER_H