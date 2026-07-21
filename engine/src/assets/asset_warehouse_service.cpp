#include "engine/assets/asset_warehouse_service.h"
#include "engine/assets/asset_data.h"
#include "engine/assets/asset_helpers.h"
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
	// assumes the asset generator tool has already been ran, so the
	// runtime asset metadata is already generated and stored in the source asset metadata files 
	
	sourceMetadatas = assetMetadataService.LoadAssetMetadata(assetRoot);
	
	// iterates through all runtime metadata
	for (const auto& [id, metadata] : sourceMetadatas) {
		for (const auto& runtimeMetadata : metadata.assetMetadatas) {
			// store the runtime metadata in the warehouse's maps
			StoreRuntimeMetadata(runtimeMetadata);
		}
	}
}

/**
 * @brief stores runtime metadata in the warehouse's maps and checks for duplicate export names.
 * 
 * @param runtimeMetadata 
 *
 * @details
 * Note that duplicate filepaths are not checked for
 *
 * This is because i think it's fine and I dont want to :)
 */
void AssetWarehouseService::StoreRuntimeMetadata(const RuntimeAssetMetadata& runtimeMetadata) {
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
	runtimeMetadatas[runtimeMetadata.id] = runtimeMetadata;

	// DO SOURCE ID BECAUSE IT MAKES SENSE, WHY WOULD YOU STORE RUNTIME UUID IF
	// YOUR ONLY USE CASE ONLY REQUIRES A SOURCE UUID DUMMY
	filePathToUUIDMap[runtimeMetadata.path] = runtimeMetadata.sourceId;
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
const SourceAssetMetadata* AssetWarehouseService::FindSourceMetadataReadOnly(UUID id) const {
	auto iterator = sourceMetadatas.find(id);
	if (iterator == sourceMetadatas.end()) {
		return nullptr;
	}

	return &iterator->second;
}

/**
 * @brief Finds the runtime metadata for the asset with the given runtime UUID.
 * 
 * @param runtimeAssetUUID The runtime UUID of the asset to find metadata for.
 * @return A pointer to the runtime asset metadata, or nullptr if not found.
 */
RuntimeAssetMetadata* AssetWarehouseService::FindRuntimeMetadata(UUID runtimeAssetUUID) {
	auto runtimeMetadataIt = runtimeMetadatas.find(runtimeAssetUUID);
	if (runtimeMetadataIt == runtimeMetadatas.end()) {
		return nullptr;
	}

	return &runtimeMetadataIt->second;
}

const RuntimeAssetMetadata* AssetWarehouseService::FindRuntimeMetadataReadOnly(UUID runtimeAssetUUID) const {
	auto runtimeMetadataIt = runtimeMetadatas.find(runtimeAssetUUID);
	if (runtimeMetadataIt == runtimeMetadatas.end()) {
		return nullptr;
	}

	return &runtimeMetadataIt->second;
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
 * @brief They call it the dependency resolver.
 */
SourceAssetMetadata AssetWarehouseService::DependencyResolver(const std::filesystem::path& assetPath) {
	if (filePathToUUIDMap.find(assetPath) != filePathToUUIDMap.end()) {
		return sourceMetadatas[filePathToUUIDMap[assetPath]];
	} else {
		throw std::runtime_error("Asset not found in warehouse: " + assetPath.string());
	}
}

/**
 * @brief Stores a loaded asset in the warehouse.
 * @param sourceMetadata The source metadata of the asset to store.
 * @param asset The asset to store.
 */
void AssetWarehouseService::StoreLoadedAsset(SourceAssetMetadata& sourceMetadata, std::unique_ptr<Asset> asset) {
	if (!asset) {
		throw std::runtime_error("Cannot store null asset.");
	}

	/*
	if no runtime metadata is associated with the generated asset, generate it and store it
	
	every source asset goes through this process at least once,
	but this is also useful for sub-assets that don't have a source asset metadata file
	like meshes and materials that are part of a model asset

	this block also populates the export name to UUID map
	*/
	RuntimeAssetMetadata* runtimeMetadata;
	runtimeMetadata = sourceMetadata.TryGetSubAssetMetadata(asset->name);
	if (runtimeMetadata == nullptr) {
		RuntimeAssetMetadata newRuntimeMetadata = assetMetadataService.GenerateRuntimeAssetMetadataNew(
			*asset,
			sourceMetadata,
			asset->name
		);
		StoreRuntimeMetadata(newRuntimeMetadata);
		sourceMetadata.assetMetadatas.push_back(newRuntimeMetadata);
		assetMetadataService.WriteMetadataAndUUID(sourceMetadata, sourceMetadata.path);		
	}
	runtimeMetadata = sourceMetadata.TryGetSubAssetMetadata(asset->name);
	assert(runtimeMetadata != nullptr); // should never be null after the above block
	ApplyMetadataToAsset(
		*runtimeMetadata,
		*asset
	);

	// flag the asset as loaded and store it in the proper maps
	sourceMetadatas[sourceMetadata.id].loaded = true; // due to how importers work, importing one runtime asset
													// guarantees all runtime assets in the source asset 
													// metadata have been loaded  
	runtimeMetadatas[asset->id].loaded = true;
	loadedAssets.insert_or_assign(asset->id, std::move(asset));
}

/**
 * @brief Clears all loaded assets and metadata from the warehouse.
 */
void AssetWarehouseService::Clear() {
	sourceMetadatas.clear();
	loadedAssets.clear();
	runtimeMetadatas.clear();
}
