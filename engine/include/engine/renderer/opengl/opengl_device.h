#pragma once

#include "engine/core/window.h"
#include "engine/renderer/graphics_device.h"

struct OpenGLConfig {};

class OpenGLDevice : public GraphicsDevice {
public:
    void ConfigureWindow() override;
    bool Init(Window& window) override;
    void BeginFrame() override;
    void Submit(UUID mesh) override;
    void Render() override;
    void EndFrame() override;
    void Close() override;
private:
    OpenGLConfig config;
    Window* window = nullptr;
};
