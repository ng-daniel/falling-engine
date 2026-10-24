#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "engine/renderer/renderer_structures.h"
#include "engine/core/window_manager.h"
#include "engine/renderer/graphics_device.h"

struct OpenGLConfig {};

struct OpenGLDeviceData : IGraphicsDeviceData{
    uint VAO = 0;
    uint VBO = 0;
    uint EBO = 0;
    uint instancedMeshVBO = 0;
    std::uint32_t indexCount = 0;
    std::uint64_t vertexCount = 0;
};

struct OpenGLShaderProgram : IGraphicsDeviceShader {
    unsigned int programId = 0;
    int modelUniform = -1;
    int viewUniform = -1;
    int projectionUniform = -1;
};

class OpenGLDevice : public GraphicsDevice {
public:
    void ConfigureWindow() override;
    bool Init(WindowManager& window) override;
    void BeginFrame() override;
    RenderStats Render(const RenderData& renderData) override;
    void EndFrame() override;
    void Close() override;

    void InitializeGPUBuffersForMesh(MeshRenderData& renderData, const MeshAsset * meshAsset) override;

    SPDEVICE_RID CreateShaderProgram(
        const std::string& vertexSource,
        const std::string& fragmentSource
    ) override;
    void DestroyShaderProgram(SPDEVICE_RID programId) override;

    TDEVICE_RID CreateTexture(
        const TextureAsset& texture,
        const ImageAsset& image
    ) override;
    void DestroyTexture(TDEVICE_RID textureId) override;
    
private:
    OpenGLConfig config;
    WindowManager* window = nullptr;
    std::unordered_map<SPDEVICE_RID, std::unique_ptr<OpenGLShaderProgram>> shaderPrograms;
    std::unordered_set<TDEVICE_RID> textures;
    
    OpenGLShaderProgram * FindShaderProgram(SPDEVICE_RID programId);
};
