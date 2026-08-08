#include "engine/assets/asset_warehouse_service.h"
#include "engine/assets/asset_data.h"
#include "engine/assets/asset_helpers.h"
#include <string>
#include <unordered_map>

#include "engine/debug/logger.h"

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
		StoreSourceMetadata(metadata);
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
		Logger::Error(
			"AssetWarehouseService::StoreRuntimeMetadata",
			"Duplicate export name '" + runtimeMetadata.exportName
				+ "' found for assets with IDs '" + std::to_string(exportNameIterator->second)
				+ "' and '" + std::to_string(runtimeMetadata.id)
				+ "'. Please rename one of the assets' export names in their respective asset metadata files and rerun the asset header generator."
		);
	}
	exportNameToUUIDMap[runtimeMetadata.exportName] = runtimeMetadata.id;
	runtimeMetadatas[runtimeMetadata.id] = runtimeMetadata;
}

void AssetWarehouseService::StoreSourceMetadata(const SourceAssetMetadata& sourceMetadata) {
	sourceMetadatas.emplace(sourceMetadata.id, sourceMetadata);
	filePathToUUIDMap[sourceMetadata.path] = sourceMetadata.id;
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
 * @brief Finds the source metadata directly by its own (source) UUID, mutable version.
 * @param sourceAssetId The UUID of the source asset itself (not a runtime asset UUID).
 * @return A pointer to the source asset metadata, or nullptr if not found.
 */
SourceAssetMetadata* AssetWarehouseService::FindSourceMetadataById(UUID sourceAssetId) {
	auto iterator = sourceMetadatas.find(sourceAssetId);
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
SourceAssetMetadata * AssetWarehouseService::DependencyResolver(const std::filesystem::path& assetPath) {
	if (filePathToUUIDMap.find(assetPath) != filePathToUUIDMap.end()) {
		return &sourceMetadatas[filePathToUUIDMap[assetPath]];
	} else {
		return nullptr;
	}
}

/**
 * @brief Stores a loaded asset in the warehouse and returns the stored pointer.
 * @param sourceMetadata The source metadata of the asset to store.
 * @param asset The asset to store.
 */
const Asset* AssetWarehouseService::StoreAsset(SourceAssetMetadata& sourceMetadata, std::unique_ptr<Asset> asset) {
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
	const UUID assetId = asset->id;

	// flag the asset as loaded and store it in the proper maps
	sourceMetadatas[sourceMetadata.id].loaded = true; // due to how importers work, importing one runtime asset
													// guarantees all runtime assets in the source asset 
													// metadata have been loaded  
	runtimeMetadatas[asset->id].loaded = true;
	loadedAssets.insert_or_assign(assetId, std::move(asset));

	return loadedAssets.at(assetId).get();
}

/**
 * @brief Clears all loaded assets and metadata from the warehouse.
 */
void AssetWarehouseService::Clear() {
	sourceMetadatas.clear();
	loadedAssets.clear();
	runtimeMetadatas.clear();
}

/**
 * @brief Returns the UUIDs of every source asset currently tracked by the warehouse.
 * Useful for GUI/CLI clients that need to enumerate the library without touching internal maps.
 */
std::vector<UUID> AssetWarehouseService::GetAllSourceAssetIds() const {
	std::vector<UUID> ids;
	ids.reserve(sourceMetadatas.size());
	for (const auto& [id, metadata] : sourceMetadatas) {
		ids.push_back(id);
	}
	return ids;
}

/**
 * @brief Registers a new source asset file with the warehouse and persists its metadata.
 * @param assetPath Path to the asset file, which must already exist on disk under the asset root
 * and must not already be tracked by the warehouse.
 * @return The UUID assigned to the new source asset.
 */
UUID AssetWarehouseService::ImportNewSourceAsset(const std::filesystem::path& assetPath) {
	if (filePathToUUIDMap.find(assetPath) != filePathToUUIDMap.end()) {
		throw std::runtime_error("Asset is already registered in the warehouse: " + assetPath.string());
	}

	SourceAssetMetadata metadata = assetMetadataService.GenerateSourceMetadata(assetPath);
	assetMetadataService.WriteMetadataAndUUID(metadata, assetPath);

	const UUID sourceId = metadata.id;
	filePathToUUIDMap[assetPath] = sourceId;
	
	StoreSourceMetadata(metadata);

	return sourceId;
}

/**
 * @brief Renames a runtime asset's export name (the user-facing name used for header generation).
 * Does not affect the underlying source file or the asset's stable sub-asset identifier.
 */
bool AssetWarehouseService::RenameRuntimeAsset(UUID runtimeAssetId, const std::string& newExportName) {
	auto runtimeIt = runtimeMetadatas.find(runtimeAssetId);
	if (runtimeIt == runtimeMetadatas.end()) {
		Logger::Warning(
			"AssetWarehouseService::RenameRuntimeAsset",
			"Runtime asset not found: " + std::to_string(runtimeAssetId)
		);
		return false;
	}

	auto existingNameIt = exportNameToUUIDMap.find(newExportName);
	if (existingNameIt != exportNameToUUIDMap.end() && existingNameIt->second != runtimeAssetId) {
		Logger::Error(
			"AssetWarehouseService::RenameRuntimeAsset",
			"Export name '" + newExportName + "' is already in use by another asset."
		);
		return false;
	}

	RuntimeAssetMetadata& runtimeMetadata = runtimeIt->second;
	auto sourceIt = sourceMetadatas.find(runtimeMetadata.sourceId);
	if (sourceIt == sourceMetadatas.end()) {
		Logger::Error(
			"AssetWarehouseService::RenameRuntimeAsset",
			"Runtime asset has no owning source asset metadata: " + std::to_string(runtimeAssetId)
		);
		return false;
	}
	SourceAssetMetadata& sourceMetadata = sourceIt->second;

	exportNameToUUIDMap.erase(runtimeMetadata.exportName);
	runtimeMetadata.exportName = newExportName;
	exportNameToUUIDMap[newExportName] = runtimeAssetId;

	// keep the copy stored inside the owning source metadata's assetMetadatas vector in sync
	RuntimeAssetMetadata* subAssetMetadata = sourceMetadata.TryGetSubAssetMetadata(runtimeMetadata.subAssetIdentifier);
	if (subAssetMetadata) {
		subAssetMetadata->exportName = newExportName;
	}

	// keep an already-loaded asset instance's cached name in sync too
	auto loadedIt = loadedAssets.find(runtimeAssetId);
	if (loadedIt != loadedAssets.end()) {
		loadedIt->second->name = newExportName;
	}

	assetMetadataService.WriteMetadataAndUUID(sourceMetadata, sourceMetadata.path);
	return true;
}

/**
 * @brief Moves a source asset's underlying file (and its .fmeta metadata file) to a new path.
 * @note If the new path changes the file's stem, sub-asset identifiers generated from the old
 * stem will not be retroactively updated; this is a pre-existing limitation of how sub-asset
 * identity is derived, not something introduced here.
 */
bool AssetWarehouseService::MoveSourceAsset(UUID sourceAssetId, const std::filesystem::path& newPath) {
	auto sourceIt = sourceMetadatas.find(sourceAssetId);
	if (sourceIt == sourceMetadatas.end()) {
		Logger::Warning(
			"AssetWarehouseService::MoveSourceAsset",
			"Source asset not found: " + std::to_string(sourceAssetId)
		);
		return false;
	}
	SourceAssetMetadata& sourceMetadata = sourceIt->second;

	if (std::filesystem::exists(newPath)) {
		Logger::Error(
			"AssetWarehouseService::MoveSourceAsset",
			"Destination path already exists: " + newPath.string()
		);
		return false;
	}

	const std::filesystem::path oldPath = sourceMetadata.path;
	const std::filesystem::path oldMetadataPath = assetMetadataService.GenerateMetadataFilePath(oldPath);
	const std::filesystem::path newMetadataPath = assetMetadataService.GenerateMetadataFilePath(newPath);

	std::error_code ec;
	std::filesystem::create_directories(newPath.parent_path(), ec);

	std::filesystem::rename(oldPath, newPath, ec);
	if (ec) {
		Logger::Error(
			"AssetWarehouseService::MoveSourceAsset",
			"Failed to move asset file from '" + oldPath.string() + "' to '" + newPath.string() + "': " + ec.message()
		);
		return false;
	}

	filePathToUUIDMap.erase(oldPath);
	sourceMetadata.path = newPath;
	filePathToUUIDMap[newPath] = sourceAssetId;

	// write metadata at the new location first, then remove the old metadata file
	assetMetadataService.WriteMetadataAndUUID(sourceMetadata, newPath);
	std::filesystem::remove(oldMetadataPath, ec);

	return true;
}

/**
 * @brief Deletes a source asset's underlying file, metadata file, and all associated
 * runtime/loaded asset bookkeeping.
 */
bool AssetWarehouseService::DeleteSourceAsset(UUID sourceAssetId) {
	auto sourceIt = sourceMetadatas.find(sourceAssetId);
	if (sourceIt == sourceMetadatas.end()) {
		Logger::Warning(
			"AssetWarehouseService::DeleteSourceAsset",
			"Source asset not found: " + std::to_string(sourceAssetId)
		);
		return false;
	}

	// copy out before erasing, since we still need the data (path, sub-assets) after removal
	SourceAssetMetadata sourceMetadata = sourceIt->second;

	for (const RuntimeAssetMetadata& runtimeMetadata : sourceMetadata.assetMetadatas) {
		loadedAssets.erase(runtimeMetadata.id);
		exportNameToUUIDMap.erase(runtimeMetadata.exportName);
		runtimeMetadatas.erase(runtimeMetadata.id);
	}

	filePathToUUIDMap.erase(sourceMetadata.path);
	sourceMetadatas.erase(sourceIt);

	std::error_code ec;
	std::filesystem::remove(sourceMetadata.path, ec);
	std::filesystem::remove(assetMetadataService.GenerateMetadataFilePath(sourceMetadata.path), ec);

	return true;
}

/**
 * @brief Unloads a single runtime asset from memory, without touching its metadata.
 */
void AssetWarehouseService::UnloadAsset(UUID runtimeAssetId) {
	auto runtimeIt = runtimeMetadatas.find(runtimeAssetId);
	if (runtimeIt != runtimeMetadatas.end()) {
		runtimeIt->second.loaded = false;
	}
	loadedAssets.erase(runtimeAssetId);
}

/**
 * @brief Unloads every runtime asset associated with a source asset.
 */
void AssetWarehouseService::UnloadSourceAsset(UUID sourceAssetId) {
	auto sourceIt = sourceMetadatas.find(sourceAssetId);
	if (sourceIt == sourceMetadatas.end()) {
		return;
	}

	for (const RuntimeAssetMetadata& runtimeMetadata : sourceIt->second.assetMetadatas) {
		UnloadAsset(runtimeMetadata.id);
	}
	sourceIt->second.loaded = false;
}
