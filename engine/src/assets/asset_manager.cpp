#include <iostream>

#include "engine/assets/asset_manager.h"

#include "engine/assets/texture_importer.h"
#include "engine/assets/shader_importer.h"

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
    ProcessAssetDirectory(rootDirectory);
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
 * Skips metadata files, and ignores files that cannot be processed by any importer.
 * 
 * If an importer is found, it calls the ImportAsset method to import the asset.
 * The imported asset's metadata is then stored in the assetMetadatas map.
 */
void AssetManager::ProcessAssetDirectory(const std::filesystem::path& assetDirectory) {

    std::cout << "Processing asset directory: " << assetDirectory.string() << std::endl;
    
    if (!std::filesystem::exists(assetDirectory)) {
        throw std::runtime_error("Asset directory does not exist: " + assetDirectory.string());
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(assetDirectory)) {
        if (entry.is_regular_file()) {
            
            const std::filesystem::path& filePath = entry.path();
            const std::string extension = filePath.extension().string();
            if (extension == ASSET_METADATA_EXTENSION) {
                continue;
            }

            std::cout << "Found asset file: " << filePath.string() << std::endl;
        
            // first load if metadata exists
            std::filesystem::path metadataFilePath = GenerateMetadataFilePath(filePath);
            if (std::filesystem::exists(metadataFilePath)) {
                try {
                    std::cout << "Reading metadata for asset: " << filePath.string() << std::endl;
                    AssetMetadata metadata = ReadMetadata(metadataFilePath);
                    ValidateMetadata(metadata, metadataFilePath);
                    assetMetadatas.emplace(metadata.id, metadata);
                } catch (const std::runtime_error& e) {
                    std::cout << "Warning: Failed to read metadata for asset, regenerating: " << filePath.string() << ". " << e.what() << std::endl;
                }
            }
            else {
                // generate new metadata file
                try {
                    std::cout << "Generating metadata for asset: " << filePath.string() << std::endl;
                    AssetMetadata metadata = GenerateMetadata(filePath);
                    WriteMetadata(metadata, filePath);
                    assetMetadatas.emplace(metadata.id, metadata);
                } catch (const std::runtime_error& e) {
                    // don't throw if a file can't be processed
                    std::cout << "Warning: Failed to process asset: " << filePath.string() << ". " << e.what() << std::endl;
                }
            }
        }
    }
}

/**
 * @brief Imports an asset from the specified file path and stores it in the loadedAssets map.
 * @param metadata The metadata of the asset to import.
 */
void AssetManager::ImportSourceAsset(AssetMetadata& metadata) {
    try {
        std::vector<std::unique_ptr<Asset>> assets;
        switch (GetAssetTypeFromString(metadata.type)) {
            case Asset::AssetType::Texture:
                assets = TextureImporter::LoadAsset(metadata.path);
                break;
            case Asset::AssetType::Shader:
                assets = ShaderImporter::LoadAsset(metadata.path);
                break;
            case Asset::AssetType::Model:
                // assets = ModelImporter::LoadAsset(metadata.path);
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
    asset->id = metadata.id;
    asset->name = metadata.path.stem().string();
    asset->type = GetAssetTypeFromString(metadata.type);
    loadedAssets.emplace(asset->id, std::move(asset));
    metadata.loaded = true;
}

/**
 * @brief Generates metadata for an asset located at the specified path.
 * @param assetPath The path to the asset file.
 * @return An AssetMetadata object containing the generated metadata.
 */
AssetMetadata AssetManager::GenerateMetadata(const std::filesystem::path& assetPath) {
    AssetMetadata metadata;
    metadata.id = UUIDGenerator::GenerateUUID();
    metadata.path = assetPath;
    try {
        std::string extension = assetPath.extension().string();
        metadata.type = GetStringFromAssetType(GetAssetTypeFromExtension(extension));
        metadata.importer = "DEPRECIATED";
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Failed to generate metadata for asset: " + assetPath.string() + ". " + e.what());
    }

    return metadata;
}

/**
 * @brief Parses metadata from a metadata file located at the specified path.
 * @param metadataFilePath The path to the metadata file.
 * @return An AssetMetadata object containing the parsed metadata.
 */
AssetMetadata AssetManager::ReadMetadata(const std::filesystem::path& metadataFilePath) {
    AssetMetadata metadata;
    try {
        metadataSerializer.Read(metadata, metadataFilePath);
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Failed to parse metadata from file: " + metadataFilePath.string() + ". " + e.what());
    }
    return metadata;
}

/**
 * @brief Validates the metadata of an asset, writing any necessary updates.
 * @param metadata The AssetMetadata object to validate.
 * @param metadataFilePath The path to the metadata file.
 */
void AssetManager::ValidateMetadata(AssetMetadata& metadata, const std::filesystem::path& metadataFilePath) {
    bool changed = false;

    // check if the asset path in the metadata matches the actual asset path
    // done in case the asset was moved
    std::string realAssetPath = metadataFilePath.string().substr(
        0, metadataFilePath.string().size() - ASSET_METADATA_EXTENSION.size()
    );
    std::cout << "\tReal asset path: " << realAssetPath << std::endl;
    std::cout << "\tRecorded asset path: " << metadata.path << std::endl;
    if (realAssetPath != metadata.path) {
        std::cout << "\tWarning: Asset path in metadata file does not match actual asset path. Updating metadata path." << std::endl;
        metadata.path = realAssetPath;
        changed = true;
    }

    if (changed) {
        WriteMetadata(metadata, realAssetPath);
    }
}

/**
 * @brief Writes metadata for an asset located at the specified path.
 * @param metadata The AssetMetadata object to write.
 * @param assetPath The path to the asset file.
 */
void AssetManager::WriteMetadata(const AssetMetadata& metadata, const std::filesystem::path& assetPath) {
    std::filesystem::path metadataFilePath = GenerateMetadataFilePath(assetPath);
    try {
        metadataSerializer.Write(metadata, metadataFilePath);
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Failed to write metadata to file: " + metadataFilePath.string() + ". " + e.what());
    }
}

/**
 * @brief Generates the file path for the metadata file corresponding to the given asset path.
 * @param assetPath The path to the asset file.
 * @return The generated file path for the metadata file.
 */
std::filesystem::path AssetManager::GenerateMetadataFilePath(const std::filesystem::path& assetPath) {
    std::filesystem::path metadataFilePath = assetPath;
    metadataFilePath += ASSET_METADATA_EXTENSION;
    return metadataFilePath;
}