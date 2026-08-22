#pragma once

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

#include "engine/assets/asset_structures.h"
#include "engine/assets/asset_metadata_service.h"

/**
 * @brief Manages the storage and retrieval of assets and their metadata.
 */
class AssetWarehouseService {
public:
	AssetWarehouseService(const std::filesystem::path& assetRoot);

	SourceAssetMetadata* FindSourceMetadata(UUID runtimeAssetUUID);
	const SourceAssetMetadata* FindSourceMetadataReadOnly(UUID id) const;
	SourceAssetMetadata* FindSourceMetadataById(UUID sourceAssetId);
	RuntimeAssetMetadata* FindRuntimeMetadata(UUID runtimeAssetUUID);
	const RuntimeAssetMetadata* FindRuntimeMetadataReadOnly(UUID runtimeAssetUUID) const;

	std::unordered_map<std::string, UUID> GetAllExportNameUUIDMappings() { return exportNameToUUIDMap; };
	std::unordered_map<UUID, RuntimeAssetMetadata> GetAllRuntimeMetadatas() { return runtimeMetadatas; };
	std::unordered_map<UUID, SourceAssetMetadata>& GetAllSourceMetadatasAsReference() { return sourceMetadatas; };
	std::vector<UUID> GetAllSourceAssetIds() const;

	bool HasLoadedAsset(UUID id) const;

	Asset* GetLoadedAsset(UUID id);
	const Asset* GetLoadedAssetReadOnly(UUID id) const;
	SourceAssetMetadata* DependencyResolver(const std::filesystem::path& assetPath);
	void StoreRuntimeMetadata(const RuntimeAssetMetadata& metadata);
	void StoreSourceMetadata(const SourceAssetMetadata& sourceMetadata);

	const Asset* StoreAsset(SourceAssetMetadata& metadata, std::unique_ptr<Asset> asset);
	void Clear();

	/**
	 * @brief Registers a brand new source asset file (already copied/placed on disk under
	 * the asset root) with the warehouse, generating and persisting its metadata.
	 * @return The UUID of the newly created source asset.
	 */
	UUID ImportNewSourceAsset(const std::filesystem::path& assetPath);

	/**
	 * @brief Renames the export name of a runtime asset (the user-facing/display name used
	 * for asset header generation). Does not touch the underlying source file.
	 * @return false if the runtime asset doesn't exist or the name is already in use.
	 */
	bool RenameRuntimeAsset(UUID runtimeAssetId, const std::string& newExportName);

	/**
	 * @brief Moves a source asset's underlying file (and its .fmeta metadata file) to a new path.
	 * @return false if the source asset doesn't exist or the move failed.
	 */
	bool MoveSourceAsset(UUID sourceAssetId, const std::filesystem::path& newPath);

	/**
	 * @brief Deletes a source asset's underlying file, metadata file, and all associated
	 * runtime/loaded asset bookkeeping.
	 * @return false if the source asset doesn't exist.
	 */
	bool DeleteSourceAsset(UUID sourceAssetId);

	/**
	 * @brief Unloads a single runtime asset from memory, without touching its metadata.
	 */
	void UnloadAsset(UUID runtimeAssetId);

	/**
	 * @brief Unloads every runtime asset associated with a source asset.
	 */
	void UnloadSourceAsset(UUID sourceAssetId);

private:
	AssetMetadataService assetMetadataService;
	
	std::unordered_map<std::string, UUID> exportNameToUUIDMap;
	std::unordered_map<std::filesystem::path, UUID> filePathToUUIDMap;

	std::unordered_map<UUID, SourceAssetMetadata> sourceMetadatas;
	std::unordered_map<UUID, RuntimeAssetMetadata> runtimeMetadatas;
	std::unordered_map<UUID, std::unique_ptr<Asset>> loadedAssets;
};
