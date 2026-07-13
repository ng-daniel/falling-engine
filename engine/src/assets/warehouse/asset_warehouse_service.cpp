#include "engine/assets/warehouse/asset_warehouse_service.h"

/**
 * @brief Constructs an AssetWarehouseService with the given asset root path.
 * @param assetRoot The root path where asset metadata is located.
 * @details
 * Initializes the asset metadata service and loads all asset metadata from the specified root path.
 */
AssetWarehouseService::AssetWarehouseService(const std::filesystem::path& assetRoot)
	: assetMetadatas(assetMetadataService.LoadAssetMetadata(assetRoot)) {}

/**
 * @brief Finds the metadata for the asset with the given ID.
 * @param id The ID of the asset to find metadata for.
 * @return A pointer to the asset metadata, or nullptr if not found.
 */
AssetMetadata* AssetWarehouseService::FindMetadata(UUID id) {
	auto iterator = assetMetadatas.find(id);
	if (iterator == assetMetadatas.end()) {
		return nullptr;
	}

	return &iterator->second;
}

/**
 * @brief Finds the metadata for the asset with the given ID (const version).
 * @param id The ID of the asset to find metadata for.
 * @return A const pointer to the asset metadata, or nullptr if not found.
 */
const AssetMetadata* AssetWarehouseService::FindMetadataReadOnly(UUID id) const {
	auto iterator = assetMetadatas.find(id);
	if (iterator == assetMetadatas.end()) {
		return nullptr;
	}

	return &iterator->second;
}

/**
 * @brief Checks if an asset with the given ID is loaded in the warehouse.
 * @param id The ID of the asset to check.
 * @return True if the asset is loaded, false otherwise.
 */
bool AssetWarehouseService::HasLoadedAsset(UUID id) const {
	return loadedAssets.find(id) != loadedAssets.end();
}

/**
 * @brief Retrieves the loaded asset with the given ID.
 * @param id The ID of the asset to retrieve.
 * @return A pointer to the loaded asset, or nullptr if not found.
 */
Asset* AssetWarehouseService::GetLoadedAsset(UUID id) {
	auto iterator = loadedAssets.find(id);
	if (iterator == loadedAssets.end()) {
		return nullptr;
	}

	return iterator->second.get();
}

/**
 * @brief Retrieves the loaded asset with the given ID (const version).
 * @param id The ID of the asset to retrieve.
 * @return A const pointer to the loaded asset, or nullptr if not found.
 */
const Asset* AssetWarehouseService::GetLoadedAssetReadOnly(UUID id) const {
	auto iterator = loadedAssets.find(id);
	if (iterator == loadedAssets.end()) {
		return nullptr;
	}

	return iterator->second.get();
}

/**
 * @brief Stores a loaded asset in the warehouse.
 * @param metadata The metadata of the asset to store.
 * @param asset The asset to store.
 */
void AssetWarehouseService::StoreLoadedAsset(AssetMetadata& metadata, std::unique_ptr<Asset> asset) {
	metadata.loaded = true;
	loadedAssets.insert_or_assign(asset->id, std::move(asset));
}

/**
 * @brief Clears all loaded assets and metadata from the warehouse.
 */
void AssetWarehouseService::Clear() {
	assetMetadatas.clear();
	loadedAssets.clear();
}
