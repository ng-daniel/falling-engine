#ifndef ENGINE_ASSETS_ASSET_MANAGER_H
#define ENGINE_ASSETS_ASSET_MANAGER_H

#include <filesystem>
#include <type_traits>

#include "engine/assets/asset_handle.h"
#include "engine/assets/asset_warehouse_service.h"
#include "engine/assets/asset_importer_service.h"


/**
 * @brief Manages asset imports and serves asset requests through the warehouse.
 */
class AssetManager {
public:
    AssetManager(std::filesystem::path root);
    ~AssetManager() = default;

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

    AssetWarehouseService& GetAssetWarehouseService() {
        return assetWarehouseService;
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

    AssetImporterService assetImporterService;
    AssetWarehouseService assetWarehouseService;
};

#endif // ENGINE_ASSETS_ASSET_MANAGER_H