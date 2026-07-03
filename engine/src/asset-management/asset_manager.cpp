#include "engine/asset-management/asset_manager.h"

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
 * Creates the necessary data structures for managing assets.
 */
AssetManager::AssetManager() {
    AssetManager::assetMetadatas = std::unordered_map<AssetID, AssetMetadata>();
    AssetManager::loadedAssets = std::unordered_map<AssetID, Asset>();

    // TODO: create asset importer subclass for textures first, then the rest
    // AssetManager::extensionToImporter = std::unordered_map<std::string, AssetImporter&>();
}

/**
 * @brief Destructs the AssetManager instance.
 * Cleans up all loaded assets and clears the asset metadata.
 */
AssetManager::~AssetManager() {
    assetMetadatas.clear();
    for (auto& [id, asset] : loadedAssets) {
        delete &asset;
    }
    loadedAssets.clear();
}

/**
 * @brief Requests an asset of the specified type and ID.
 * 
 * @tparam T The type of the asset to request.
 * @param id The ID of the asset to request.
 * 
 * @return A reference to the requested asset.
 * 
 * @details
 * This is the main interface for external code to request assets from the AssetManager.
 * First searches the loadedAssets map for the asset.
 * If not found, it loads the asset from disk by looking it up
 * in the assetMetadatas map and adds it to the loadedAssets map.
 */
template <typename T>
T& AssetManager::RequestAsset(AssetID id) {
    return nullptr;
}

/**
 * @brief Populates assetMetadatas by scanning the 
 * specified asset directory and importing assets.
 * 
 * @param assetDirectory The directory to scan for assets.
 * 
 * @details
 * Walks through the specified asset directory and its subdirectories.
 * For each file found, it checks if the file extension has a corresponding
 * AssetImporter registered in the extensionToImporter map.
 * 
 * If an importer is found, it calls the ImportAsset method to import the asset.
 * The imported asset's metadata is then stored in the assetMetadatas map.
 */
void AssetManager::ProcessAssetDirectory(const std::filesystem::path& assetDirectory) {
    return;
}

/**
 * @brief Imports an asset from the specified file path.
 * 
 */
std::vector<Asset>
AssetManager::ImportSourceAsset(AssetMetadata& metadata) {
    return {};
}

/**
 * @brief Retrieves the appropriate AssetImporter for the given file extension.
 * @param extension The file extension to look up.
 * @return A reference to the corresponding AssetImporter.
 */
AssetImporter& AssetManager::GetImporterForExtension(const std::string& extension) {

}

AssetMetadata AssetManager::GenerateMetadata(const std::filesystem::path& assetPath) {
    return AssetMetadata();
}

AssetMetadata AssetManager::ParseMetadata(const std::filesystem::path& metadataFilePath) {
    return AssetMetadata();
}

AssetID AssetManager::GenerateSourceAssetID() {
    return 0;
}

AssetID AssetManager::GenerateSubAssetID(AssetID parentID, const std::string& subAssetName) {
    return 0;
}