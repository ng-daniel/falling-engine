#pragma once

#include <memory>
#include <vector>

#include "engine/utils/uuid.h"
#include "engine/renderer/graphics_device.h"
#include "engine/utils/matrix.h"

#include "engine/assets/asset_data.h"
#include "engine/assets/asset_manager.h"

class WindowManager;

class Renderer {
public:
    Renderer(AssetManager& assetManager);
    ~Renderer();

    void ConfigureWindow();
    bool Init(WindowManager& window);
    void BeginFrame();
    void SubmitMesh(UUID meshId, ECS_RID entityId, Matrix4 matrix);
    void Render();
    void EndFrame();
private:
    std::unique_ptr<GraphicsDevice> device;
    RenderData renderData;
    AssetManager& assetManagerRef;
};
