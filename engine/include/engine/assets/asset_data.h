#ifndef ENGINE_ASSETS_ASSET_DATA_H
#define ENGINE_ASSETS_ASSET_DATA_H

#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>

#include "engine/ecs/ecs_data.h"
#include "engine/utils/uuid.h"

/**
 * @brief Public wrapper for asset metadata.
 */
struct AssetInfo {
    UUID id;                     // source/runtime GUID based on context
    UUID sourceId;               // source GUID, identical to id for source assets
    std::string name;            // display/export name
    std::string type;            // type as string
    std::filesystem::path path;  // source file path
    bool loaded = false;         // is it loaded in memory (runtime assets only) ?
};

struct RuntimeAssetMetadata {
    UUID id;
    UUID sourceId;                  // reference to the source asset metadata
    std::string exportName;         // unique name for the asset, editable by user, used to generate asset header file
    std::string subAssetIdentifier; // unique name for the asset, used for sub-assets
    std::string type;               // string representation of the asset type
    bool loaded = false;                    // whether the asset is already loaded into memory
};

struct SourceAssetMetadata {
    UUID id;
    std::string type;
    std::filesystem::path path;
    std::vector<RuntimeAssetMetadata> assetMetadatas; // INCLUDES SOURCE ASSET METADATA
    bool loaded = false;

    RuntimeAssetMetadata* TryGetSubAssetMetadata(const std::string& subAssetIdentifier) {
        for (auto& runtimeMetadata : assetMetadatas) {
            if (runtimeMetadata.subAssetIdentifier == subAssetIdentifier) {
                return &runtimeMetadata;
            }
        }
        return nullptr;
    }

    static RuntimeAssetMetadata* GetPrimaryRuntimeMetadata(SourceAssetMetadata& sourceMetadata) {
        const std::string primarySubAssetIdentifier = sourceMetadata.path.stem().string();
        return sourceMetadata.TryGetSubAssetMetadata(primarySubAssetIdentifier);
    }
};

/**
 * @brief Struct for assets, should not be used by itself.
 * But also can't be abstract because it is returned by AssetLoaders.
 */
struct Asset {    
    enum AssetType {
        Model,
        Mesh,
        Material,
        Texture,
        Image,
        Shader
    };
    UUID id;
    std::string name;
    AssetType type;
};

#endif // ENGINE_ASSETS_ASSET_DATA_H