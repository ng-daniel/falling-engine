#pragma once

#include "engine/utils/uuid.h"
#include "engine/ecs/ecs_runtime_id.h"

#include "engine/assets/asset_data.h"
#include "engine/utils/matrix.h"
#include <unordered_map>

enum GraphicsAPI {
    OpenGL,
    Vulkan
};

/**
 * @brief Interface for graphics devices to define their own API specific data structures
 */
struct IGraphicsDeviceData {};

struct PrimitiveRenderData {
    const MeshAsset * mesh;
    int pIdx; // primitive index in the mesh data array
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
    const MeshRenderData * meshRenderData;
    Matrix4 transform;
};

struct RenderData {
    // meshes map to assets
    std::unordered_map<UUID, MeshRenderData> meshRenderData;
    std::unordered_map<ECS_RID, MeshTransformData> meshTransformData;
};