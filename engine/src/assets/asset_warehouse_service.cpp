#include "engine/assets/asset_warehouse_service.h"
#include <iostream>
#include <string>
#include <unordered_map>

/**
 * @brief Constructs an AssetWarehouseService with the given asset root path.
 * @param assetRoot The root path where asset metadata is located.
 * @details
 * Initializes the asset metadata service and loads all asset metadata from the specified root path.
 */
AssetWarehouseService::AssetWarehouseService(const std::filesystem::path& assetRoot) {
	// populate metadata maps (all of them except loadedAssets)
	sourceMetadatas = assetMetadataService.LoadAssetMetadata(assetRoot);
	for (const auto& [id, metadata] : sourceMetadatas) {
		for (const auto& runtimeMetadata : metadata.assetMetadatas) {
			if (exportNameToUUIDMap.find(runtimeMetadata.exportName) != exportNameToUUIDMap.end()) {
				std::string existingAssetPath = sourceMetadatas[exportNameToUUIDMap[runtimeMetadata.exportName]].path.string();
				throw std::runtime_error("Error loading asset metadata: Duplicate export name '" + runtimeMetadata.exportName +
					"' found for assets '" + existingAssetPath + "' and '" + metadata.path.string() + "'." +
					"Please rename one of the assets' export names in their respective asset metadata files.");
			}
			exportNameToUUIDMap[runtimeMetadata.exportName] = runtimeMetadata.id;
			runtimeMetadatas[runtimeMetadata.id] = runtimeMetadata;
		}
	}
}

/**
 * @brief Finds the source metadata for the asset with the given runtime UUID.
 * @param id The ID of the asset to find metadata for.
 * @return A pointer to the asset metadata, or nullptr if not found.
 */
SourceAssetMetadata* AssetWarehouseService::FindSourceMetadata(UUID runtimeAssetUUID) {
	auto runtimeMetadataIt = runtimeMetadatas.find(runtimeAssetUUID);
	if (runtimeMetadataIt == runtimeMetadatas.end()) {
		return nullptr;
	}

	UUID sourceId = runtimeMetadataIt->second.sourceId;
	auto sourceMetadataIt = sourceMetadatas.find(sourceId);
	if (sourceMetadataIt == sourceMetadatas.end()) {
		return nullptr;
	}

	return &sourceMetadataIt->second;
}

/**
 * @brief Finds the metadata for the asset with the given ID (const version).
 * @param id The ID of the asset to find metadata for.
 * @return A const pointer to the asset metadata, or nullptr if not found.
 */
const SourceAssetMetadata* AssetWarehouseService::FindMetadataReadOnly(UUID id) const {
	auto iterator = sourceMetadatas.find(id);
	if (iterator == sourceMetadatas.end()) {
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
void AssetWarehouseService::StoreLoadedAsset(SourceAssetMetadata& metadata, std::unique_ptr<Asset> asset) {
	if (!asset) {
		throw std::runtime_error("Cannot store null asset.");
	}

	const UUID assetId = asset->id;
	const std::string exportName = asset->name;
	const Asset::AssetType assetType = asset->type;

	// if no runtime metadata is associated with the generated asset, generate it and store it

	if (runtimeMetadatas.find(assetId) == runtimeMetadatas.end()) {
		
		RuntimeAssetMetadata runtimeMetadata = assetMetadataService.GenerateRuntimeAssetMetadata(
			assetId,
			exportName,
			assetType,
			metadata);

		auto exportNameIterator = exportNameToUUIDMap.find(runtimeMetadata.exportName);
		if (exportNameIterator != exportNameToUUIDMap.end() && exportNameIterator->second != runtimeMetadata.id) {
			std::cerr << "Error: Duplicate export name '" << runtimeMetadata.exportName
					  << "' found for assets with IDs '" << exportNameIterator->second
					  << "' and '" << runtimeMetadata.id << "'."
					  << "  Please rename one of the assets' export names in their respective asset metadata files."
					  << " and rerun the asset header generator."
					  << std::endl;
		}

		exportNameToUUIDMap[runtimeMetadata.exportName] = runtimeMetadata.id;
		metadata.assetMetadatas.push_back(runtimeMetadata);
		assetMetadataService.WriteMetadataAndUUID(metadata, metadata.path);
		
		runtimeMetadatas[runtimeMetadata.id] = runtimeMetadata;
	}
	metadata.loaded = true;
	loadedAssets.insert_or_assign(assetId, std::move(asset));
}

/**
 * @brief Clears all loaded assets and metadata from the warehouse.
 */
void AssetWarehouseService::Clear() {
	sourceMetadatas.clear();
	loadedAssets.clear();
}
