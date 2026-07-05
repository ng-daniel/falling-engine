#ifndef ENGINE_ASSETS_ASSET_HELPERS_H
#define ENGINE_ASSETS_ASSET_HELPERS_H

#include "asset_data.h"
#include <stdexcept>

Asset::AssetType GetAssetTypeFromString(const std::string& typeStr) {
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

std::string GetStringFromAssetType(Asset::AssetType type) {
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

#endif // ENGINE_ASSETS_ASSET_HELPERS_H