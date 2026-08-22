#ifndef ENGINE_ASSETS_ASSET_DATA_H
#define ENGINE_ASSETS_ASSET_DATA_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "external/json.hpp"

#include "engine/assets/asset_structures.h"
#include "engine/utils/uuid.h"
#include "engine/utils/vector.h"

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
    UUID image;

    enum class FilterType {
        Undefined = 0,
        Nearest = 9728,
        Linear = 9729,
        NearestMipmapNearest = 9984,
        LinearMipmapNearest = 9985,
        NearestMipmapLinear = 9986,
        LinearMipmapLinear = 9987
    };

    enum class WrapMode {
        ClampToEdge = 33071,
        MirroredRepeat = 33648,
        Repeat = 10497
    };

    FilterType mag_filter = FilterType::Undefined;
    FilterType min_filter = FilterType::Undefined;
    WrapMode wrap_s = WrapMode::Repeat;
    WrapMode wrap_t = WrapMode::Repeat;
};

struct MaterialAsset : public Asset {
    enum class MaterialType {
        PBRMetallicRoughness,
        Unlit,
        Custom
    };
    MaterialType materialType;
    Vector4 baseColorFactor = Vector4(1.0f);

    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    float occlusionFactor = 1.0f;
    float normalFactor = 1.0f;

    Vector3 emissiveFactor = Vector3(0.0f);

    UUID baseColorTexture;
    UUID metallicRoughnessTexture;
    UUID normalTexture;
    UUID occlusionTexture;
    UUID emissiveTexture;
};

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector4 tangent;
    Vector2 texCoord;
};

struct MeshAsset : public Asset {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

struct ModelAsset : public Asset {
    std::vector<UUID> meshes;
    std::vector<UUID> materials;
};

class ComponentData {
    std::string componentType;
    nlohmann::json componentData;
};

class EntityData {
    UUID entityId;
    std::vector<ComponentData> components;
};

class SceneAsset : public Asset {
    UUID rootEntity;
    std::vector<EntityData> entities;
};

#endif // ENGINE_ASSETS_ASSET_DATA_H