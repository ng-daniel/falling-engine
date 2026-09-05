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
    void SubmitMesh(UUID meshId, const Matrix4& worldTransform);
    void Render();
    void EndFrame();

    /// SHADER STUFF
    /// --------------------

    SHADER_RID RegisterShaderProgram(UUID vertexShaderId, UUID fragmentShaderId);
    void SetDefaultShaderProgram(SHADER_RID shaderProgramId);

private:
    AssetManager& assetManagerRef;
    std::unique_ptr<GraphicsDevice> device;

    RenderData frameData;
    std::unordered_map<UUID, MeshRenderData> meshCache;
    std::unordered_map<SHADER_RID, ShaderProgramData> shaderCache;
    SHADER_RID defaultShaderProgram = INVALID_SH_RID;

    /// SHADER STUFF
    /// --------------------

    MeshRenderData* GetOrCreateMeshRenderData(UUID meshId);
    bool HasShaderProgram(SHADER_RID shaderProgramId) const;
    SHADER_RID nextShaderProgramId = 1; // next available shader program ID
};
