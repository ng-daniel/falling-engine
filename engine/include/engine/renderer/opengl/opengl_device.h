#pragma once

#include <memory>
#include <unordered_map>

#include "engine/renderer/renderer_structures.h"
#include "engine/core/window_manager.h"
#include "engine/renderer/graphics_device.h"

struct OpenGLConfig {};

struct OpenGLDeviceData : IGraphicsDeviceData{
    uint VAO;
    uint VBO;
    uint EBO;
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
    void Render(RenderData& renderData) override;
    void EndFrame() override;
    void Close() override;

    SPDEVICE_RID CreateShaderProgram(
        const std::string& vertexSource,
        const std::string& fragmentSource
    ) override;
    void DestroyShaderProgram(SPDEVICE_RID programId) override;
    
private:
    OpenGLConfig config;
    WindowManager* window = nullptr;
    std::unordered_map<SPDEVICE_RID, std::unique_ptr<OpenGLShaderProgram>> shaderPrograms;
    
    OpenGLShaderProgram * FindShaderProgram(SPDEVICE_RID programId);
};