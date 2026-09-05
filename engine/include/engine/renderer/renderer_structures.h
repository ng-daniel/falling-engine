#pragma once

#include "engine/utils/uuid.h"
#include "engine/ecs/ecs_runtime_id.h"

#include "engine/assets/asset_data.h"
#include "engine/utils/matrix.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

enum GraphicsAPI {
    OpenGL,
    Vulkan
};

/**
 * @brief Interface for graphics devices to define their own API specific data structures
 */
struct IGraphicsDeviceData {
    virtual ~IGraphicsDeviceData() = default;
};

/**
 * @brief Interface for graphics device shaders to define their own API specific shader structures
 */
struct IGraphicsDeviceShader {
    virtual ~IGraphicsDeviceShader() = default;
};

using SHADER_RID = std::uint32_t; // ID for generic renderer data entry
constexpr SHADER_RID INVALID_SH_RID = 0;

using SPDEVICE_RID = uint; // ID for graphics device shader program
constexpr SPDEVICE_RID INVALID_SPDEVICE_RID = 0;

struct ShaderProgramData {
    SHADER_RID id;
    SPDEVICE_RID deviceProgramId = INVALID_SPDEVICE_RID;
    
    UUID vertexShaderId = 0;
    UUID fragmentShaderId = 0;
    
    const ShaderAsset * vertexShader;
    const ShaderAsset * fragmentShader;
};

struct PrimitiveRenderData {
    const MeshAsset * mesh;
    int pIdx; // primitive index in the mesh data array
    UUID materialId = 0;
    std::unique_ptr<IGraphicsDeviceData> graphicsDeviceData;
};

/**
 * @brief Stores render data for a single mesh
 */
struct MeshRenderData {
    UUID meshId;
    bool initialized = false;
    const MeshAsset * mesh;
    std::vector<PrimitiveRenderData> primitives;
};

struct MeshTransformData {
    ECS_RID entityId; // entity ID associated with this mesh transform data
    const MeshRenderData * meshRenderData; // the mesh render data to transform
    Matrix4 worldTransform;
};

struct RenderSubmission {
    MeshTransformData * meshTransformData;
    ShaderProgramData * shaderProgram;
};

struct RenderData {
    std::unordered_map<UUID, MeshRenderData> meshRenderData;
    std::unordered_map<ECS_RID, MeshTransformData> meshTransformData;
    std::unordered_map<SHADER_RID, ShaderProgramData> shaderPrograms;
    std::vector<RenderSubmission> submissions;
};