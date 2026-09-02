#pragma once

#include "engine/utils/uuid.h"

class Renderer {
public:
    void BeginFrame();
    void Submit(UUID mesh);
    void Render();
    void EndFrame();
};