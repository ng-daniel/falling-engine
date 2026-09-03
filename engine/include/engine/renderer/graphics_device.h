#pragma once

#include "engine/utils/uuid.h"
#include "engine/renderer/renderer_structures.h"

class Window;

class GraphicsDevice {
public:
    virtual ~GraphicsDevice() = default;
    virtual void ConfigureWindow() = 0;
    virtual bool Init(Window& window) = 0;
    virtual void BeginFrame() = 0;
    virtual void Submit(UUID mesh) = 0;
    virtual void Render() = 0;
    virtual void EndFrame() = 0;
    virtual void Close() = 0;
};
