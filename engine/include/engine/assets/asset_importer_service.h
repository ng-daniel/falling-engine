#ifndef ENGINE_ASSETS_ASSET_IMPORTER_SERVICE_H
#define ENGINE_ASSETS_ASSET_IMPORTER_SERVICE_H

#include "engine/assets/asset_data.h"
#include "engine/assets/asset_warehouse_service.h"
#include <vector>
#include <memory>

class AssetImporterService {
public:
    AssetImporterService() = default;
    ~AssetImporterService() = default;

    std::vector<std::unique_ptr<Asset>> ImportSourceAsset(
        SourceAssetMetadata& metadata,
        AssetWarehouseService& assetWarehouseService
    );
};

#endif // ENGINE_ASSETS_ASSET_IMPORTER_SERVICE_H