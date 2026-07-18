#include "engine/assets/asset_warehouse_service.h"

#include "generator.h"

/**
 * @brief 
 * 
 * @param assetRootDir 
 * @param outputDir 
 */
void AssetHeaderGenerator::Generate(const std::string& assetRootDir, const std::string& outputDir) {
    AssetWarehouseService assetWarehouseService(assetRootDir);
    return;
}