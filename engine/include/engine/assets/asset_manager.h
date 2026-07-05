#ifndef ENGINE_ASSETS_ASSET_MANAGER_H
#define ENGINE_ASSETS_ASSET_MANAGER_H

#include <cstdint>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

#include "engine/serialization/asset_metadata_serializer.h"

#include "asset_structures.h"
#include "texture_importer.h"
    
/**
 * @brief Manages all loading, storage, and distribution of assets for the game.
 */
class AssetManager {
public:
    AssetManager(std::filesystem::path root);
    ~AssetManager();

    /**
     * @brief Requests an asset of the specified type and ID.
     * @tparam T The type of the asset to request.
     * @param id The ID of the asset to request.
     * @return A reference to the requested asset.
     */
    template<typename T>
    T& RequestAsset(UUID id);

private:
    const std::string ASSET_METADATA_EXTENSION = ".fmeta"; // stands for falling metadata
    std::filesystem::path rootDirectory;

    // asset importers
    TextureImporter textureImporter;

    // metadata serializer
    AssetMetadataSerializer metadataSerializer;

    // data storage
    std::unordered_map<UUID, AssetMetadata> assetMetadatas;
    std::unordered_map<UUID, std::unique_ptr<Asset>> loadedAssets;

    // mappings
    std::unordered_map<std::string, std::reference_wrapper<AssetImporter>> extensionToImporter;

    void ProcessAssetDirectory(const std::filesystem::path& assetDirectory);

    std::unique_ptr<Asset> 
    ImportSourceAsset(AssetMetadata& metadata);
    
    AssetImporter& GetImporterForExtension(const std::string& extension);
    AssetImporter& GetImporterByName(const std::string& importerName);
    
    AssetMetadata GenerateMetadata(const std::filesystem::path& assetPath);
    AssetMetadata ReadMetadata(const std::filesystem::path& metadataFilePath);
    void ValidateMetadata(AssetMetadata& metadata, const std::filesystem::path& metadataFilePath);
    void WriteMetadata(const AssetMetadata& metadata, const std::filesystem::path& assetPath);
    std::filesystem::path GenerateMetadataFilePath(const std::filesystem::path& assetPath);

    UUID GenerateSourceAssetID();
    UUID GenerateSubAssetID(UUID parentID, const std::string& subAssetName);
};

#endif // ENGINE_ASSETS_ASSET_MANAGER_H