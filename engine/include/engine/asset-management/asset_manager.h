#include <iostream>
#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <vector>

using AssetID = std::uint32_t;

enum AssetType {
    Model,
    Mesh,
    Material,
    Texture,
    Shader
};

struct AssetMetadata {
    AssetID id;
    std::string type;
    std::filesystem::path path;
    bool loaded; // in CPU memory or not
};

/// @brief Base class for assets.
class Asset {
public:
    AssetID id;
    std::string name;
};

/// @brief Manages loading, storage, and access of assets in the engine.
class AssetManager {
public:
    
    /// @brief Requests an asset of the specified type and ID.
    /// @tparam T The type of the asset to request.
    /// @param id The ID of the asset to request.
    /// @return A reference to the requested asset.
    template<typename T>
    T& RequestAsset(AssetID id);

    /// @brief Populates assetMetadatas by scanning the 
    /// specified asset directory and importing assets.
    /// @param assetDirectory The directory to scan for assets.
    void ProcessAssets(const std::filesystem::path& assetDirectory);

private:
    
    /// @brief Stores the metadata for all assets managed by the AssetManager.
    std::unordered_map<AssetID, AssetMetadata> assetMetadatas;
    /// @brief Stores the loaded assets in memory.
    std::unordered_map<AssetID, std::unique_ptr<Asset>> loadedAssets;    

    void ImportAsset(const std::filesystem::path& path);
    
    /// @brief Generates metadata for an asset, assigning type based on file type.
    /// @return The generated asset metadata.
    AssetMetadata GenerateMetadata();

    /// @brief Parses a metadata file for an asset.
    /// @param metadataFilePath The path to the metadata file.
    /// @return The parsed asset metadata.
    AssetMetadata ParseMetadata(const std::filesystem::path& metadataFilePath);

};
