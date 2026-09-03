#pragma once

#include "engine/renderer/renderer_structures.h"

class WindowManager;

class GraphicsDevice {
public:
    virtual ~GraphicsDevice() = default;

    virtual void ConfigureWindow() = 0;
    virtual bool Init(WindowManager& window) = 0;
    virtual void Close() = 0;

    virtual void BeginFrame() = 0;
    virtual void Render(RenderData& renderData) = 0;
    virtual void EndFrame() = 0;
};
