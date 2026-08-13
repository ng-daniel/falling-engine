#ifndef ENGINE_ASSETS_ASSET_MANAGER_H
#define ENGINE_ASSETS_ASSET_MANAGER_H

#include <filesystem>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "engine/assets/asset_handle.h"
#include "engine/assets/asset_warehouse_service.h"
#include "engine/assets/asset_importer_service.h"

/**
 * @brief Public API for asset management: importing, loading, unloading, reimporting,
 * renaming, moving, deleting, and querying metadata.
 *
 * This is the single entry point that GUI/CLI/test code should use. Internal systems
 * (AssetWarehouseService, AssetImporterService, AssetMetadataService) are intentionally not
 * exposed so that clients cannot mutate asset state without going through this API.
 */
class AssetManager {
public:
    AssetManager(std::filesystem::path root);
    ~AssetManager() = default;

    /// Loading = put existing asset into memory
    /// -------------------------------------------------------------------------

    template <typename T>
    T* RequestAsset(UUID id) {
        return static_cast<T*>(RequestAsset(id, GetRequestedAssetType<T>()));
    }
    template <typename T>
    T* RequestAsset(AssetHandle id) {
        return RequestAsset<T>(id.GetUUID());
    }

    /**
     * @brief Requests an asset of the specified type and ID, but returns a const pointer.
     * @tparam T The type of the asset to request.
     * @param id The ID of the asset to request.
     * @return A const pointer to the requested asset, or nullptr if not found.
     */
    template <typename T>
    const T* RequestAssetReadOnly(UUID id) {
        return RequestAsset<T>(id);
    }
    template <typename T>
    const T* RequestAssetReadOnly(AssetHandle id) {
        return RequestAssetReadOnly<T>(id.GetUUID());
    }

    /// Importing = add new file to asset directory
    /// -------------------------------------------------------------------------

    /**
     * @brief Imports a new asset file from outside the library into it. Copies the file into
     * the asset root and registers metadata for it. The asset is not eagerly loaded into
     * memory; it will be imported lazily the first time it is requested.
     * @param externalFilePath Path to the file to bring into the library.
     * @return The UUID of the new source asset, or std::nullopt on failure.
     */
    std::optional<UUID> ImportAsset(const std::filesystem::path& externalFilePath);

    /**
     * @brief Re-runs the importer for a source asset, refreshing all of its runtime sub-assets.
     * Existing runtime asset UUIDs are preserved where possible, so existing references
     * (e.g. AssetHandle constants) remain valid.
     * @param sourceAssetId The UUID of the source asset to reimport.
     * @return true on success.
     */
    bool ReimportAsset(UUID sourceAssetId);

    /// Unloading = remove from memory, but keep metadata
    /// -------------------------------------------------------------------------

    /**
     * @brief Unloads a single runtime asset from memory. Metadata is preserved.
     * @param runtimeAssetId The UUID of the runtime asset to unload.
     */
    void UnloadAsset(UUID runtimeAssetId);

    /**
     * @brief Unloads every runtime asset belonging to a source asset.
     * @param sourceAssetId The UUID of the source asset whose runtime assets should be unloaded.
     */
    void UnloadSourceAsset(UUID sourceAssetId);

    /// Modifying and Deletion
    /// -------------------------------------------------------------------------

    /**
     * @brief Renames a runtime asset's user-facing export name (used for header generation).
     * Does not touch the underlying source file.
     */
    bool RenameAsset(UUID runtimeAssetId, const std::string& newExportName);
    /**
     * @brief Moves a source asset's underlying file to a new path within the library.
     * @param sourceAssetId The UUID of the source asset to move.
     * @param newPath The new path within the library.
     * @return true on success.
     */
    bool MoveAsset(UUID sourceAssetId, const std::filesystem::path& newPath);
    /**
     * @brief Deletes a source asset's underlying file and all associated metadata/state.
     * @param sourceAssetId The UUID of the source asset to delete.
     * @return true on success.
     */
    bool DeleteAsset(UUID sourceAssetId);

    // Metadata Querying
    /// -------------------------------------------------------------------------

    std::vector<AssetInfo> GetAllSourceAssets() const;
    std::optional<AssetInfo> GetSourceAssetInfo(UUID sourceAssetId) const;
    std::vector<AssetInfo> GetRuntimeAssetsForSource(UUID sourceAssetId) const;
    AssetInfo GetPrimaryRuntimeAssetForSource(UUID sourceAssetId) const;
    std::optional<AssetInfo> GetRuntimeAssetInfo(UUID runtimeAssetId) const;
    std::filesystem::path GetAssetRoot() const {
        return assetRoot;
    }

private:
    template <typename T>
    static constexpr Asset::AssetType GetRequestedAssetType() {
        if constexpr (std::is_same_v<T, ModelAsset>) {
            return Asset::AssetType::Model;
        } else if constexpr (std::is_same_v<T, MeshAsset>) {
            return Asset::AssetType::Mesh;
        } else if constexpr (std::is_same_v<T, MaterialAsset>) {
            return Asset::AssetType::Material;
        } else if constexpr (std::is_same_v<T, TextureAsset>) {
            return Asset::AssetType::Texture;
        } else if constexpr (std::is_same_v<T, ImageAsset>) {
            return Asset::AssetType::Image;
        } else if constexpr (std::is_same_v<T, ShaderAsset>) {
            return Asset::AssetType::Shader;
        } else {
            static_assert(!sizeof(T*), "Unsupported asset type requested from AssetManager.");
        }
    }

    Asset* RequestAsset(UUID id, Asset::AssetType expectedType);

    std::filesystem::path assetRoot;
    AssetImporterService assetImporterService;
    AssetWarehouseService assetWarehouseService;
};

#endif // ENGINE_ASSETS_ASSET_MANAGER_H