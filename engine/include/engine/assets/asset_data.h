#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "external/json.hpp"

#include "engine/assets/asset_structures.h"
#include "engine/utils/uuid.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

struct ShaderAsset : public Asset {
    enum ShaderType {
        Vertex,
        Fragment
    };
    ShaderType shaderType;
    std::string shaderSource;
};

struct ImageAsset : public Asset {
    int width = 0;
    int height = 0;
    int numChannels = 0;
    std::vector<unsigned char> data;
};

struct TextureAsset : public Asset {
    UUID image = INVALID_UUID;

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
    MaterialType materialType = MaterialType::PBRMetallicRoughness;
    glm::vec4 baseColorFactor = glm::vec4(1.0f);

    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    float occlusionFactor = 1.0f;
    float normalFactor = 1.0f;

    glm::vec3 emissiveFactor = glm::vec3(0.0f);

    UUID baseColorTexture = INVALID_UUID;
    UUID metallicRoughnessTexture = INVALID_UUID;
    UUID normalTexture = INVALID_UUID;
    UUID occlusionTexture = INVALID_UUID;
    UUID emissiveTexture = INVALID_UUID;

    // Runtime-only link to a ShaderProgramData. Asset serialization deliberately
    // does not include this field.
    UUID runtimeShader = INVALID_UUID;
};

struct Vertex {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec4 tangent = glm::vec4(0.0f);
    glm::vec2 texCoord = glm::vec2(0.0f);
};

/**
 * @brief GLTF spec says meshes are composed of primitives
 * https://github.khronos.org/glTF-Tutorials/gltfTutorial/gltfTutorial_009_Meshes.html
 */
struct PrimitiveData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    UUID material = 0;
};

struct MeshAsset : public Asset {
    std::vector<PrimitiveData> primitives;
};

struct ModelNode {
    UUID meshId = 0;
    bool hasMesh = false;
    std::string name;
    glm::vec3 relativePosition = glm::vec3(0.0f);
    glm::quat relativeRotation = glm::quat::wxyz(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 relativeScale = glm::vec3(1.0f);
    std::vector<ModelNode> children;
};

struct ModelAsset : public Asset {
    std::vector<UUID> meshes; // all mesh references contained in the scene hierarchies
    std::vector<ModelNode> trees; // scene hierarchies
};

struct ComponentData {
    std::string type;
    nlohmann::json data;
};

struct EntityData {
    UUID entityId;
    std::string name;
    std::vector<ComponentData> components;
};

struct SceneAsset : public Asset {
    UUID rootEntity;
    std::vector<EntityData> entities;
};
