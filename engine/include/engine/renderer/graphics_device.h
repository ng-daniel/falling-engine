#pragma once

#include "engine/assets/asset_data.h"
#include "engine/renderer/renderer_structures.h"
#include "engine/renderer/render_stats.h"

class WindowManager;

class GraphicsDevice {
public:
    virtual ~GraphicsDevice() = default;

    virtual void ConfigureWindow() = 0;
    virtual bool Init(WindowManager& window) = 0;
    virtual void Close() = 0;

    virtual void BeginFrame() = 0;
    virtual RenderStats Render(const RenderData& renderData) = 0;
    virtual void EndFrame() = 0;

    virtual void InitializeGPUBuffersForMesh(MeshRenderData& renderData, const MeshAsset * meshAsset) = 0;
    
    virtual SPDEVICE_RID CreateShaderProgram(
        const std::string& vertexSource,
        const std::string& fragmentSource
    ) = 0;
    virtual void DestroyShaderProgram(SPDEVICE_RID programId) = 0;

    virtual TDEVICE_RID CreateTexture(
        const TextureAsset& texture,
        const ImageAsset& image
    ) = 0;
    virtual void DestroyTexture(TDEVICE_RID textureId) = 0;
};
