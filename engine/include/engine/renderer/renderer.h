#pragma once

#include <memory>

#include "engine/utils/uuid.h"
#include "engine/renderer/graphics_device.h"

class WindowManager;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void ConfigureWindow();
    bool Init(WindowManager& window);
    void BeginFrame();
    void Submit(UUID mesh);
    void Render();
    void EndFrame();
private:
    std::unique_ptr<GraphicsDevice> device;
};
