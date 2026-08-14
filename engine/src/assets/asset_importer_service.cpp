#include "engine/assets/asset_importer_service.h"

#include "engine/assets/asset_structures.h"

#include "engine/assets/importers/image_importer.h"
#include "engine/assets/importers/texture_importer.h"
#include "engine/assets/importers/shader_importer.h"
#include "engine/assets/importers/model_importer.h"

#include "engine/assets/asset_helpers.h"

/**
 * @brief Imports a source asset based on its metadata and the asset warehouse service.
 * 
 * @param metadata Metadata of the source asset to be imported.
 * @param assetWarehouseService Only required sometimes, specifically for Model imports since it 
 * depends on other assets, namely image assets that may already exist in the warehouse.
 * @return std::vector<const Asset*> 
 */

std::vector<const Asset*> AssetImporterService::ImportSourceAsset(
    SourceAssetMetadata& metadata,
    AssetWarehouseService& assetWarehouseService
) {
    try {
        std::vector<const Asset*> assets;
        switch (GetAssetTypeFromString(metadata.type)) {
            case Asset::AssetType::Texture:
                assets.push_back(TextureImporter::LoadAsset(metadata, assetWarehouseService));
                break;
            case Asset::AssetType::Shader:
                assets.push_back(ShaderImporter::LoadAsset(metadata, assetWarehouseService));
                break;
            case Asset::AssetType::Model:
                assets.push_back(ModelImporter::LoadAsset(metadata, assetWarehouseService));
                break;
            default:
                throw std::runtime_error("Unsupported asset type for import: " + metadata.type);
        }
        return assets;

    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Failed to import asset: " + metadata.path.string() + ". " + e.what());
    }
}