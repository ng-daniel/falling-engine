#ifndef ENGINE_ASSETS_ASSET_HELPERS_H
#define ENGINE_ASSETS_ASSET_HELPERS_H

#include "asset_data.h"
#include "engine/utils/uuid.h"
#include <stdexcept>
#include <string>

inline Asset::AssetType GetAssetTypeFromString(const std::string& typeStr) {
    if (typeStr == "Model") {
        return Asset::AssetType::Model;
    } else if (typeStr == "Mesh") {
        return Asset::AssetType::Mesh;
    } else if (typeStr == "Material") {
        return Asset::AssetType::Material;
    } else if (typeStr == "Texture") {
        return Asset::AssetType::Texture;
    } else if (typeStr == "Image") {
        return Asset::AssetType::Image;
    } else if (typeStr == "Shader") {
        return Asset::AssetType::Shader;
    } else {
        throw std::runtime_error("Unknown asset type string: " + typeStr);
    }
}

inline std::string GetStringFromAssetType(Asset::AssetType type) {
    switch (type) {
        case Asset::AssetType::Model:
            return "Model";
        case Asset::AssetType::Mesh:
            return "Mesh";
        case Asset::AssetType::Material:
            return "Material";
        case Asset::AssetType::Texture:
            return "Texture";
        case Asset::AssetType::Image:
            return "Image";
        case Asset::AssetType::Shader:
            return "Shader";
        default:
            throw std::runtime_error("Unknown asset type enum value");
    }
}

inline Asset::AssetType GetAssetTypeFromExtension(const std::string& extension) {
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg") {
        return Asset::AssetType::Image;
    } else if (extension == ".glb" || extension == ".gltf") {
        return Asset::AssetType::Model;
    } else if (extension == ".vert" || extension == ".frag") {
        return Asset::AssetType::Shader;
    } else {
        throw std::runtime_error("Unknown asset type for extension: " + extension);
    }
}

/**
 * @brief Applies metadata information to an asset object.
 * @param runtimeMetadata The runtime metadata containing the information to apply
 * @param asset The asset object to which the metadata will be applied.
 */
inline void ApplyMetadataToAsset(
    RuntimeAssetMetadata& runtimeMetadata,
    Asset& asset) {
    
    asset.id = runtimeMetadata.id;
    asset.name = runtimeMetadata.exportName;
    asset.type = GetAssetTypeFromString(runtimeMetadata.type);
}
inline void ApplyMetadataToAsset(
    SourceAssetMetadata& sourceMetadata,
    Asset& asset) {
    
    asset.id = sourceMetadata.id;
    asset.name = sourceMetadata.path.stem().string();
    asset.type = GetAssetTypeFromString(sourceMetadata.type);
}

#endif // ENGINE_ASSETS_ASSET_HELPERS_H