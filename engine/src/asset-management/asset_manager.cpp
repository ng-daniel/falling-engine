#include "engine/asset-management/asset_manager.h"
#include "engine/asset-management/texture_importer.h"

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
    textureImporter = TextureImporter();
    
    extensionToImporter.emplace(".jpg", textureImporter);
    extensionToImporter.emplace(".png", textureImporter);
}

/**
 * @brief Destructs the AssetManager instance.
 * Cleans up all loaded assets and clears the asset metadata.
 */
AssetManager::~AssetManager() {
    assetMetadatas.clear();
    loadedAssets.clear();
    extensionToImporter.clear();
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
T& AssetManager::RequestAsset(UUID id) {
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
    auto iterator = extensionToImporter.find(extension); // returns an iterator because ?? cpp is freak language
    if (iterator == extensionToImporter.end()) {
        throw std::runtime_error("No importer found for extension: " + extension);
    }
    return iterator->second.get(); // accesses the value in the key-value pair
}

/**
 * @brief Retrieves the appropriate AssetImporter by its name.
 * @param importerName The name of the importer to look up.
 * @return A reference to the corresponding AssetImporter.
 * 
 * @note
 * Needs to be manually updated whenever a new importer is added.
 */
AssetImporter& AssetManager::GetImporterByName(const std::string& importerName) {
    if (importerName == textureImporter.GetName()) {
        return textureImporter;
    }
    throw std::runtime_error("No importer found with name: " + importerName);
}

/**
 * @brief Generates metadata for an asset located at the specified path.
 * @param assetPath The path to the asset file.
 * @return An AssetMetadata object containing the generated metadata.
 */
AssetMetadata AssetManager::GenerateMetadata(const std::filesystem::path& assetPath) {
    AssetMetadata metadata;
    metadata.id = GenerateSourceAssetID();
    metadata.path = assetPath;
    try {
        AssetImporter& importer = GetImporterForExtension(assetPath.extension().string());
        metadata.importer = importer.GetName();
        metadata.type = importer.GetType();
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Failed to generate metadata for asset: " + assetPath.string() + ". " + e.what());
    }

    return metadata;
}

AssetMetadata AssetManager::ParseMetadata(const std::filesystem::path& metadataFilePath) {
    return AssetMetadata();
}

UUID AssetManager::GenerateSubAssetID(UUID parentID, const std::string& subAssetName) {
    return 0;
}