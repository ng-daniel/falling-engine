#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "engine/utils/uuid.h"
#include "engine/assets/asset_data.h"
#include "engine/utils/matrix.h"

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

/**
 * @brief ID type for generic shader programs in the renderer
 * 
 */
using SHADER_RID = std::uint32_t;
constexpr SHADER_RID INVALID_SH_RID = 0;

/**
 * @brief ID type for graphics device specific shader programs
 * 
 */
using SPDEVICE_RID = std::uint32_t;
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

/**
 * @brief A single render submission from the main loop to the renderer
 */
struct RenderSubmission {
    MeshRenderData * mesh;
    Matrix4 worldTransform;
};

/**
 * @brief Stores all render submissions for a single frame
 * Sends this data to the graphics device
 */
struct RenderData {
    std::vector<RenderSubmission> submissions;
};
