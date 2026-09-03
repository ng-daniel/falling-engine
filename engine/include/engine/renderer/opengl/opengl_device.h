#pragma once

#include "engine/core/window_manager.h"
#include "engine/renderer/graphics_device.h"

struct OpenGLConfig {};

class OpenGLDevice : public GraphicsDevice {
public:
    void ConfigureWindow() override;
    bool Init(WindowManager& window) override;
    void BeginFrame() override;
    void Submit(UUID mesh) override;
    void Render() override;
    void EndFrame() override;
    void Close() override;
private:
    OpenGLConfig config;
    WindowManager* window = nullptr;
};
