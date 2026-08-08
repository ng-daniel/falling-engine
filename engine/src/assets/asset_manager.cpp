#include "engine/assets/asset_manager.h"

#include "engine/debug/logger.h"

/*
Asset Manager Implementation
--------------------------------

On startup, initializes the asset warehouse by scanning the asset directory.
Determines the appropriate AssetImporter for each asset based on file extension.

Lazily imports assets when requested by external code and stores them in the asset warehouse.
*/

/**
 * @brief Constructs an AssetManager instance.
 * 
 * @param root The root directory where assets are located.
 * 
 * @details
 * Initializes the asset warehouse from the asset root.
 */
AssetManager::AssetManager(std::filesystem::path root)
    : assetWarehouseService(root) {}


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
Asset* AssetManager::RequestAsset(UUID id, Asset::AssetType expectedType)
{
    RuntimeAssetMetadata* metadata = assetWarehouseService.FindRuntimeMetadata(id);
    if (!metadata) {
        Logger::Warning(
            "AssetManager::RequestAsset",
            "Asset with ID " + std::to_string(id) + " not found in asset warehouse metadata."
        );
        return nullptr;
    }

    if (!assetWarehouseService.HasLoadedAsset(id)) {
        SourceAssetMetadata* sourceMetadata = assetWarehouseService.FindSourceMetadata(metadata->id);
        if (!sourceMetadata) {
            Logger::Error(
                "AssetManager::RequestAsset",
                "Asset with ID " + std::to_string(id) + " has runtime metadata but no source metadata."
            );
            return nullptr;
        }

        assetImporterService.ImportSourceAsset(*sourceMetadata, assetWarehouseService);
    }

    Asset* asset = assetWarehouseService.GetLoadedAsset(id);
    if (!asset) {
        Logger::Error(
            "AssetManager::RequestAsset",
            "Asset with ID " + std::to_string(id) + " failed to load."
        );
        return nullptr;
    }

    if (asset->type != expectedType) {
        Logger::Error(
            "AssetManager::RequestAsset",
            "Asset with ID " + std::to_string(id) + " is not of the requested type."
        );
        return nullptr;
    }

    Logger::Info(
        "AssetManager::RequestAsset",
        "Asset with ID " + std::to_string(id) + " loaded and returned."
    );
    return asset;
}