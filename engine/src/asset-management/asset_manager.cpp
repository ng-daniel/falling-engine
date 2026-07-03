#include "asset_manager.h"

/// Asset Manager Implementation
/// --------------------------------

AssetManager::AssetManager() {
    AssetManager::assetMetadatas = std::unordered_map<AssetID, AssetMetadata>();
    AssetManager::loadedAssets = std::unordered_map<AssetID, Asset&>();

    // TODO: create asset importer subclass for textures first, then the rest
    // AssetManager::extensionToImporter = std::unordered_map<std::string, AssetImporter&>();
}

AssetManager::~AssetManager() {
    assetMetadatas.clear();
    for (auto& [id, asset] : loadedAssets) {
        delete &asset;
    }
    loadedAssets.clear();
}

template <typename T>
T& AssetManager::RequestAsset(AssetID id) {
    return nullptr;
}

void AssetManager::ProcessAssets(const std::filesystem::path& assetDirectory) {
    return;
}

void AssetManager::ImportAsset(const std::filesystem::path& path) {
    return;    
}

AssetMetadata AssetManager::GenerateMetadata() {
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

