#ifndef ENGINE_ASSETS_ASSET_STRUCTURES_H
#define ENGINE_ASSETS_ASSET_STRUCTURES_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "engine/utils/uuid.h"
#include "engine/assets/asset_data.h"

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

    enum class FilterType
    {
        Undefined = 0,
        Nearest = 9728,
        Linear = 9729,
        NearestMipmapNearest = 9984,
        LinearMipmapNearest = 9985,
        NearestMipmapLinear = 9986,
        LinearMipmapLinear = 9987
    };

    enum class WrapMode
    {
        ClampToEdge = 33071,
        MirroredRepeat = 33648,
        Repeat = 10497
    };
    
    FilterType mag_filter = FilterType::Undefined;
    FilterType min_filter = FilterType::Undefined;
    WrapMode wrap_s = WrapMode::Repeat;
    WrapMode wrap_t = WrapMode::Repeat;
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
};

/**
 * @brief The actual entity data that gets serialized/deserialized
 * only used for serializing/deserializing thingAssets, not runtime
 * kinda like a translation unit
 */
class EntityData {
    UUID entityId;
};

/**
 * @brief Represents a scene/prefab (same thing in this house)
 * Gets serialized and deserialized n stuff
 */
class ThingAsset : public Asset {
    UUID rootEntity;
    std::vector<EntityData> entities;
    // still need to add the component store structures
};

#endif // ENGINE_ASSETS_ASSET_STRUCTURES_H