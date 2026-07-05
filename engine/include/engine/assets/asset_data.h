#ifndef ENGINE_ASSETS_ASSET_DATA_H
#define ENGINE_ASSETS_ASSET_DATA_H

#include <cstdint>
#include <string>

#include "engine/utils/uuid.h"

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
        Shader
    };
    UUID id;
    std::string name;
    AssetType type;
};

struct TextureAsset : public Asset {
    int width;
    int height;
    int numChannels;
    std::vector<unsigned char> data;
};

struct ShaderAsset : public Asset {    
    enum ShaderType {
        Vertex,
        Fragment
    };
    ShaderType shaderType;
    std::string shaderSource;
};

#endif // ENGINE_ASSETS_ASSET_DATA_H