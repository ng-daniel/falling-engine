#ifndef ENGINE_ASSETS_WAREHOUSE_ASSET_WAREHOUSE_SERVICE_H
#define ENGINE_ASSETS_WAREHOUSE_ASSET_WAREHOUSE_SERVICE_H

#include <filesystem>
#include <memory>
#include <unordered_map>

#include "engine/assets/asset_data.h"
#include "engine/assets/metadata/asset_metadata_service.h"

/**
 * @brief Manages the storage and retrieval of assets and their metadata.
 */
class AssetWarehouseService {
public:
	AssetWarehouseService(const std::filesystem::path& assetRoot);

	SourceAssetMetadata* FindMetadata(UUID id);
	const SourceAssetMetadata* FindMetadataReadOnly(UUID id) const;

	bool HasLoadedAsset(UUID id) const;
	Asset* GetLoadedAsset(UUID id);
	const Asset* GetLoadedAssetReadOnly(UUID id) const;

	void StoreLoadedAsset(SourceAssetMetadata& metadata, std::unique_ptr<Asset> asset);
	void Clear();

private:
	AssetMetadataService assetMetadataService;
	std::unordered_map<UUID, SourceAssetMetadata> assetMetadatas;
	std::unordered_map<UUID, std::unique_ptr<Asset>> loadedAssets;
};

#endif // ENGINE_ASSETS_WAREHOUSE_ASSET_WAREHOUSE_SERVICE_H