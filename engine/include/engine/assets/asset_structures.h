#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "engine/utils/uuid.h"

struct AssetInfo {
    UUID id;
    UUID sourceId;
    std::string name;
    std::string type;
    std::filesystem::path path;
    bool loaded = false;
};

struct RuntimeAssetMetadata {
    UUID id;
    UUID sourceId;
    std::string exportName;
    std::string subAssetIdentifier;
    std::string type;
    bool loaded = false;
};

struct SourceAssetMetadata {
    UUID id;
    std::string type;
    std::filesystem::path path;
    std::vector<RuntimeAssetMetadata> assetMetadatas;
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
