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
        case Asset::AssetType::Shader:
            return "Shader";
        default:
            throw std::runtime_error("Unknown asset type enum value");
    }
}

inline Asset::AssetType GetAssetTypeFromExtension(const std::string& extension) {
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg") {
        return Asset::AssetType::Texture;
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
 * @param id The UUID of the asset.
 * @param name The name of the asset.
 * @param type The type of the asset as a string.
 * @param asset The asset object to which the metadata will be applied.
 */
inline void ApplyMetadataToAsset(
    const UUID id, 
    const std::string name,
    const std::string type,
    Asset& asset) {
    
    asset.id = id;
    asset.name = name;
    asset.type = GetAssetTypeFromString(type);
}

#endif // ENGINE_ASSETS_ASSET_HELPERS_H