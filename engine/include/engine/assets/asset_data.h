#ifndef ENGINE_ASSETS_ASSET_DATA_H
#define ENGINE_ASSETS_ASSET_DATA_H

#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>

#include "engine/utils/uuid.h"

struct AssetMetadata {
    UUID id;
    std::string type;
    std::string importer;
    std::filesystem::path path;
    bool loaded; // in CPU memory or not
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

struct ShaderAsset : public Asset {    
    enum ShaderType {
        Vertex,
        Fragment
    };
    ShaderType shaderType;
    std::string shaderSource;
};

struct ImageAsset : public Asset {
    int width;
    int height;
    int numChannels;
    std::vector<unsigned char> data;
};

struct TextureAsset : public Asset {
    UUID image; // reference to an ImageAsset

    // arbitrary integers that r code for filtering/wrapping modes
    // every texture ref in glTF has a sampler
    // see cgltf_sampler for more info
    int mag_filter = 0;
	int min_filter;
	int wrap_s;
	int wrap_t;
};

struct MaterialAsset : public Asset
{
    enum class MaterialType
    {
        PBRMetallicRoughness,
        Unlit,
        Custom
    };
    MaterialType materialType;
    glm::vec4 baseColorFactor = glm::vec4(1.0f);

    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    float occlusionFactor = 1.0f;
    float normalFactor = 1.0f;

    glm::vec3 emissiveFactor = glm::vec3(0.0f);

    UUID baseColorTexture;          // diffuse texture
    UUID metallicRoughnessTexture;  // metallic (G), roughness (B)
    UUID normalTexture;             // adjusts lighting with normals
    UUID occlusionTexture;          // make things darker
    UUID emissiveTexture;           // make things look like glow
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 tangent;   // derive bitangent & calculate normal mapping
    glm::vec2 texCoord;
};

struct MeshAsset : public Asset
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

struct ModelAsset : public Asset
{
    std::vector<UUID> meshes;
    std::vector<UUID> materials;
    std::vector<UUID> textures;

    UUID defaultScene;
};

#endif // ENGINE_ASSETS_ASSET_DATA_H