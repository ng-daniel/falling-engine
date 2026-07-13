#include "engine/assets/asset_manager.h"

#include "engine/assets/importers/image_importer.h"
#include "engine/assets/importers/shader_importer.h"

#include "engine/assets/asset_helpers.h"

/*
Asset Manager Implementation
--------------------------------

On startup, initializes the asset warehouse by scanning the asset directory.
Determines the appropriate AssetImporter for each asset based on file extension.

Lazily imports assets when requested by external code and stores them in the asset warehouse.
*/

/**
 * @brief Constructs an AssetManager instance.
 * 
 * @param root The root directory where assets are located.
 * 
 * @details
 * Initializes the asset warehouse from the asset root.
 */
AssetManager::AssetManager(std::filesystem::path root)
    : assetWarehouseService(root) {}

/**
 * @brief Imports an asset from the specified file path and stores it in the asset warehouse.
 * @param metadata The metadata of the asset to import.
 */
void AssetManager::ImportSourceAsset(AssetMetadata& metadata) {
    try {
        std::vector<std::unique_ptr<Asset>> assets;
        switch (GetAssetTypeFromString(metadata.type)) {
            case Asset::AssetType::Image:
                assets = ImageImporter::LoadAsset(metadata);
                break;
            case Asset::AssetType::Shader:
                assets = ShaderImporter::LoadAsset(metadata);
                break;
            case Asset::AssetType::Model:
                // assets = ModelImporter::LoadAsset(metadata);
            default:
                throw std::runtime_error("Unsupported asset type for import: " + metadata.type);
        }

        for (auto& asset : assets) {
            assetWarehouseService.StoreLoadedAsset(metadata, std::move(asset));
        }
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Failed to import asset: " + metadata.path.string() + ". " + e.what());
    }
}