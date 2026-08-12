#include "engine/assets/asset_manager.h"

#include "engine/debug/logger.h"

/*
Asset Manager Implementation
--------------------------------

On startup, initializes the asset warehouse by scanning the asset directory.
Determines the appropriate AssetImporter for each asset based on file extension.

Lazily imports assets when requested by external code and stores them in the asset warehouse.

This class is the public API surface for asset management: GUI/CLI/test code should only ever
talk to AssetManager, never to AssetWarehouseService/AssetImporterService/AssetMetadataService
directly, so that internal asset-management data can only be mutated through this API.
*/

namespace {
    AssetInfo BuildAssetInfo(const SourceAssetMetadata& sourceMetadata) {
        AssetInfo info;
        info.id = sourceMetadata.id;
        info.sourceId = sourceMetadata.id;
        info.name = sourceMetadata.path.stem().string();
        info.type = sourceMetadata.type;
        info.path = sourceMetadata.path;
        info.loaded = sourceMetadata.loaded;
        return info;
    }

    AssetInfo BuildAssetInfo(const RuntimeAssetMetadata& runtimeMetadata, const SourceAssetMetadata& sourceMetadata) {
        AssetInfo info;
        info.id = runtimeMetadata.id;
        info.sourceId = runtimeMetadata.sourceId;
        info.name = runtimeMetadata.exportName;
        info.type = runtimeMetadata.type;
        info.path = sourceMetadata.path;
        info.loaded = runtimeMetadata.loaded;
        return info;
    }
}

/**
 * @brief Constructs an AssetManager instance.
 * 
 * @param root The root directory where assets are located.
 * 
 * @details
 * Initializes the asset warehouse from the asset root.
 */
AssetManager::AssetManager(std::filesystem::path root)
    : assetRoot(std::move(root)), assetWarehouseService(assetRoot) {}


/**
 * @brief Requests an asset of the specified type and ID.
 * 
 * @tparam T The type of the asset to request.
 * @param id The runtime asset ID of the asset to request.
 * 
 * @return A pointer to the requested asset, or nullptr if not found.
 * 
 * @details
 * This is the main interface for external code to request assets from the AssetManager.
 * First searches the asset warehouse for the asset.
 * If not found, it loads the asset from disk by looking up metadata
 * in the asset warehouse and storing the loaded asset there.
 */
Asset* AssetManager::RequestAsset(UUID id, Asset::AssetType expectedType)
{
    RuntimeAssetMetadata* metadata = assetWarehouseService.FindRuntimeMetadata(id);
    if (!metadata) {
        Logger::Warning(
            "AssetManager::RequestAsset",
            "Asset with ID " + std::to_string(id) + " not found in asset warehouse metadata."
        );
        return nullptr;
    }

    if (!assetWarehouseService.HasLoadedAsset(id)) {
        SourceAssetMetadata* sourceMetadata = assetWarehouseService.FindSourceMetadata(metadata->id);
        if (!sourceMetadata) {
            Logger::Error(
                "AssetManager::RequestAsset",
                "Asset with ID " + std::to_string(id) + " has runtime metadata but no source metadata."
            );
            return nullptr;
        }

        assetImporterService.ImportSourceAsset(*sourceMetadata, assetWarehouseService);
    }

    Asset* asset = assetWarehouseService.GetLoadedAsset(id);
    if (!asset) {
        Logger::Error(
            "AssetManager::RequestAsset",
            "Asset with ID " + std::to_string(id) + " failed to load."
        );
        return nullptr;
    }

    if (asset->type != expectedType) {
        Logger::Error(
            "AssetManager::RequestAsset",
            "Asset with ID " + std::to_string(id) + " is not of the requested type."
        );
        return nullptr;
    }

    Logger::Info(
        "AssetManager::RequestAsset",
        "Asset with ID " + std::to_string(id) + " loaded and returned."
    );
    return asset;
}

/**
 * @brief Imports a new asset file into the library by copying it under the asset root and
 * registering fresh metadata for it.
 */
std::optional<UUID> AssetManager::ImportAsset(const std::filesystem::path& externalFilePath) {
    if (!std::filesystem::exists(externalFilePath) || !std::filesystem::is_regular_file(externalFilePath)) {
        Logger::Error(
            "AssetManager::ImportAsset",
            "Source file does not exist or is not a regular file: " + externalFilePath.string()
        );
        return std::nullopt;
    }

    std::filesystem::path destination = assetRoot / externalFilePath.filename();
    if (std::filesystem::exists(destination)) {
        Logger::Error(
            "AssetManager::ImportAsset",
            "An asset already exists at destination: " + destination.string()
        );
        return std::nullopt;
    }

    std::error_code ec;
    std::filesystem::copy_file(externalFilePath, destination, std::filesystem::copy_options::none, ec);
    if (ec) {
        Logger::Error(
            "AssetManager::ImportAsset",
            "Failed to copy '" + externalFilePath.string() + "' into the library: " + ec.message()
        );
        return std::nullopt;
    }

    try {
        return assetWarehouseService.ImportNewSourceAsset(destination);
    } catch (const std::runtime_error& e) {
        Logger::Error("AssetManager::ImportAsset", e.what());
        std::filesystem::remove(destination);
        return std::nullopt;
    }
}

/**
 * @brief Re-runs the importer for a source asset, refreshing its runtime sub-assets in place.
 */
bool AssetManager::ReimportAsset(UUID sourceAssetId) {
    SourceAssetMetadata* sourceMetadata = assetWarehouseService.FindSourceMetadataById(sourceAssetId);
    if (!sourceMetadata) {
        Logger::Warning(
            "AssetManager::ReimportAsset",
            "Source asset not found: " + std::to_string(sourceAssetId)
        );
        return false;
    }

    assetWarehouseService.UnloadSourceAsset(sourceAssetId);

    try {
        assetImporterService.ImportSourceAsset(*sourceMetadata, assetWarehouseService);
    } catch (const std::runtime_error& e) {
        Logger::Error("AssetManager::ReimportAsset", e.what());
        return false;
    }
    return true;
}

void AssetManager::UnloadAsset(UUID runtimeAssetId) {
    assetWarehouseService.UnloadAsset(runtimeAssetId);
}

void AssetManager::UnloadSourceAsset(UUID sourceAssetId) {
    assetWarehouseService.UnloadSourceAsset(sourceAssetId);
}

bool AssetManager::RenameAsset(UUID runtimeAssetId, const std::string& newExportName) {
    return assetWarehouseService.RenameRuntimeAsset(runtimeAssetId, newExportName);
}

bool AssetManager::MoveAsset(UUID sourceAssetId, const std::filesystem::path& newPath) {
    return assetWarehouseService.MoveSourceAsset(sourceAssetId, newPath);
}

bool AssetManager::DeleteAsset(UUID sourceAssetId) {
    return assetWarehouseService.DeleteSourceAsset(sourceAssetId);
}

std::vector<AssetInfo> AssetManager::GetAllSourceAssets() const {
    std::vector<AssetInfo> result;
    for (UUID id : assetWarehouseService.GetAllSourceAssetIds()) {
        const SourceAssetMetadata* metadata = assetWarehouseService.FindSourceMetadataReadOnly(id);
        if (metadata) {
            result.push_back(BuildAssetInfo(*metadata));
        }
    }
    return result;
}

std::optional<AssetInfo> AssetManager::GetSourceAssetInfo(UUID sourceAssetId) const {
    const SourceAssetMetadata* metadata = assetWarehouseService.FindSourceMetadataReadOnly(sourceAssetId);
    if (!metadata) {
        return std::nullopt;
    }
    return BuildAssetInfo(*metadata);
}

std::vector<AssetInfo> AssetManager::GetRuntimeAssetsForSource(UUID sourceAssetId) const {
    std::vector<AssetInfo> result;
    const SourceAssetMetadata* sourceMetadata = assetWarehouseService.FindSourceMetadataReadOnly(sourceAssetId);
    if (!sourceMetadata) {
        return result;
    }
    for (const RuntimeAssetMetadata& runtimeMetadata : sourceMetadata->assetMetadatas) {
        result.push_back(BuildAssetInfo(runtimeMetadata, *sourceMetadata));
    }
    return result;
}

AssetInfo AssetManager::GetPrimaryRuntimeAssetForSource(UUID sourceAssetId) const {
    const SourceAssetMetadata* sourceMetadata = assetWarehouseService.FindSourceMetadataReadOnly(sourceAssetId);
    if (!sourceMetadata) {
        throw std::runtime_error("Source asset not found: " + std::to_string(sourceAssetId));
    }
    const RuntimeAssetMetadata* primaryRuntimeMetadata = SourceAssetMetadata::GetPrimaryRuntimeMetadata(
        *const_cast<SourceAssetMetadata*>(sourceMetadata)
    );
    if (!primaryRuntimeMetadata) {
        throw std::runtime_error("Source asset has no primary runtime asset: " + std::to_string(sourceAssetId));
    }
    return BuildAssetInfo(*primaryRuntimeMetadata, *sourceMetadata);
}

std::optional<AssetInfo> AssetManager::GetRuntimeAssetInfo(UUID runtimeAssetId) const {
    const RuntimeAssetMetadata* runtimeMetadata = assetWarehouseService.FindRuntimeMetadataReadOnly(runtimeAssetId);
    if (!runtimeMetadata) {
        return std::nullopt;
    }
    const SourceAssetMetadata* sourceMetadata = assetWarehouseService.FindSourceMetadataReadOnly(runtimeMetadata->sourceId);
    if (!sourceMetadata) {
        return std::nullopt;
    }
    return BuildAssetInfo(*runtimeMetadata, *sourceMetadata);
}