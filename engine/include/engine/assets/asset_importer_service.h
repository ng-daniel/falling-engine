#pragma once

#include "engine/assets/asset_structures.h"
#include "engine/assets/asset_warehouse_service.h"
#include <vector>

class AssetImporterService {
public:
    AssetImporterService() = default;
    ~AssetImporterService() = default;

    std::vector<const Asset*> ImportSourceAsset(
        SourceAssetMetadata& metadata,
        AssetWarehouseService& assetWarehouseService
    );
};
