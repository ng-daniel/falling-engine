#ifndef ENGINE_ASSETS_WAREHOUSE_ASSET_WAREHOUSE_SERVICE_H
#define ENGINE_ASSETS_WAREHOUSE_ASSET_WAREHOUSE_SERVICE_H

#include <filesystem>
#include <memory>
#include <unordered_map>

#include "engine/assets/asset_data.h"
#include "engine/assets/asset_metadata_service.h"

/**
 * @brief Manages the storage and retrieval of assets and their metadata.
 */
class AssetWarehouseService {
public:
	AssetWarehouseService(const std::filesystem::path& assetRoot);

	SourceAssetMetadata* FindSourceMetadata(UUID runtimeAssetUUID);
	const SourceAssetMetadata* FindSourceMetadataReadOnly(UUID id) const;
	RuntimeAssetMetadata* FindRuntimeMetadata(UUID runtimeAssetUUID);
	const RuntimeAssetMetadata* FindRuntimeMetadataReadOnly(UUID runtimeAssetUUID) const;

	std::unordered_map<std::string, UUID> GetAllExportNameUUIDMappings() { return exportNameToUUIDMap; };
	std::unordered_map<UUID, RuntimeAssetMetadata> GetAllRuntimeMetadatas() { return runtimeMetadatas; };
	std::unordered_map<UUID, SourceAssetMetadata>& GetAllSourceMetadatasAsReference() { return sourceMetadatas; };

	bool HasLoadedAsset(UUID id) const;

	Asset* GetLoadedAsset(UUID id);
	const Asset* GetLoadedAssetReadOnly(UUID id) const;
	SourceAssetMetadata DependencyResolver(const std::filesystem::path& assetPath);
	void StoreRuntimeMetadata(const RuntimeAssetMetadata& metadata);

	void StoreLoadedAsset(SourceAssetMetadata& metadata, std::unique_ptr<Asset> asset);
	void Clear();

private:
	AssetMetadataService assetMetadataService;
	
	std::unordered_map<std::string, UUID> exportNameToUUIDMap;
	std::unordered_map<std::filesystem::path, UUID> filePathToUUIDMap;

	std::unordered_map<UUID, SourceAssetMetadata> sourceMetadatas;
	std::unordered_map<UUID, RuntimeAssetMetadata> runtimeMetadatas;
	std::unordered_map<UUID, std::unique_ptr<Asset>> loadedAssets;
};

#endif // ENGINE_ASSETS_WAREHOUSE_ASSET_WAREHOUSE_SERVICE_H