#pragma once

#include "engine/renderer/renderer_structures.h"
#include "engine/core/window_manager.h"
#include "engine/renderer/graphics_device.h"

struct OpenGLConfig {};

class OpenGLDevice : public GraphicsDevice {
public:
    void ConfigureWindow() override;
    bool Init(WindowManager& window) override;
    void BeginFrame() override;
    void Render(RenderData renderData) override;
    void EndFrame() override;
    void Close() override;
private:
    OpenGLConfig config;
    WindowManager* window = nullptr;
};
