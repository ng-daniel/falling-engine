#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "engine/utils/uuid.h"
#include "engine/renderer/renderer_structures.h"
#include "engine/renderer/graphics_device.h"
#include "engine/utils/matrix.h"

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

    /// SHADER STUFF
    /// --------------------

    SHADER_RID RegisterShaderProgram(UUID vertexShaderId, UUID fragmentShaderId);
    void SetDefaultShaderProgram(SHADER_RID shaderProgramId);

private:
    AssetManager& assetManagerRef;    
    
    std::unique_ptr<GraphicsDevice> device;

    RenderData renderData;
    SHADER_RID defaultShaderProgram = INVALID_SH_RID;

    /// SHADER STUFF
    /// --------------------

    bool HasShaderProgram(SHADER_RID shaderProgramId) const;
    SHADER_RID nextShaderProgramId = 1; // next available shader program ID
};
