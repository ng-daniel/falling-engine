#include <iostream>

#include "engine/assets/asset_manager.h"

#include "engine/assets/importers/image_importer.h"
#include "engine/assets/importers/shader_importer.h"
#include "engine/assets/importers/model_importer.h"

#include "engine/assets/asset_helpers.h"

/*
Asset Manager Implementation
--------------------------------

On startup, builds assetMetadatas by walking the asset directory.
Determines the appropriate AssetImporter for each asset based on file extension.
Builds an AssetMetadata for each source asset.

Lazily loads assets into loadedAssets when requested by external code.
*/

/**
 * @brief Constructs an AssetManager instance.
 * 
 * @param root The root directory where assets are located.
 * 
 * @details
 * Initializes the necessary importers.
 * 
 * Scans the asset directory to build the assetMetadatas map.
 */
AssetManager::AssetManager(std::filesystem::path root)
    : rootDirectory(std::move(root)) {
    assetMetadatas = assetMetadataService.LoadAssetMetadata(rootDirectory);
}

/**
 * @brief Destructs the AssetManager instance.
 * Cleans up all loaded assets and clears the asset metadata.
 */
AssetManager::~AssetManager() {
    assetMetadatas.clear();
    loadedAssets.clear();
}

/**
 * @brief Imports an asset from the specified file path and stores it in the loadedAssets map.
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
            RegisterLoadedAsset(metadata, std::move(asset));
        }
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Failed to import asset: " + metadata.path.string() + ". " + e.what());
    }
}

/**
 * @brief Registers a loaded asset in the loadedAssets map and updates its metadata.
 * @param metadata The metadata of the asset to register.
 * @param asset A unique pointer to the loaded asset.
 */
void AssetManager::RegisterLoadedAsset(AssetMetadata& metadata, std::unique_ptr<Asset> asset) {
    loadedAssets.emplace(asset->id, std::move(asset));
    metadata.loaded = true;
}