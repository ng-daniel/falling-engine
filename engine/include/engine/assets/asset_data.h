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
 * @brief Represents a scene/prefab (same thing in this house)
 * Gets serialized and deserialized n stuff
 */
class ThingAsset : public Asset {
    UUID rootEntity;
    std::vector<Entity> entities;
    // still need to add the component store structures
};


#endif // ENGINE_ASSETS_ASSET_DATA_H