#pragma once

#include "engine/assets/asset_data.h"
#include "engine/utils/matrix.h"
#include <unordered_map>

enum GraphicsAPI {
    OpenGL,
    Vulkan
};

struct IGraphicsDeviceData {};

/**
 * @brief Stores render data for a single mesh
 */
struct MeshRenderData {
    UUID meshId;
    const MeshAsset * mesh;
    IGraphicsDeviceData * deviceData = nullptr;
};

struct MeshTransformData {
    const MeshRenderData * meshRenderData;
    Matrix4 transform;
};
    
struct RenderData {
    // meshes map to assets
    std::unordered_map<UUID, MeshRenderData> meshRenderData;
    std::unordered_map<UUID,MeshTransformData> meshTransformData;
};