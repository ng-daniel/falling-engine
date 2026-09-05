#include "engine/renderer/renderer.h"
#include "engine/renderer/opengl/opengl_device.h"
#include "engine/renderer/renderer_structures.h"

#include "engine/debug/logger.h"

namespace {
    PrimitiveRenderData BuildPrimitiveRenderData(const MeshAsset * parentMesh, int pIdx) {
        PrimitiveRenderData primitiveRenderData;
        primitiveRenderData.mesh = parentMesh;
        primitiveRenderData.pIdx = pIdx;
        primitiveRenderData.materialId = parentMesh->primitives[pIdx].material;
        primitiveRenderData.graphicsDeviceData = nullptr;
        return primitiveRenderData;
    }
    
    MeshRenderData BuildMeshRenderData(const MeshAsset * meshAsset) {
        MeshRenderData meshRenderData;
        meshRenderData.meshId = meshAsset->id;
        meshRenderData.mesh = meshAsset;
        return meshRenderData;
    }

    MeshTransformData BuildMeshTransformData(MeshRenderData * meshRenderData, ECS_RID entityId) {
        MeshTransformData meshTransformData;
        meshTransformData.meshRenderData = meshRenderData;
        meshTransformData.entityId = entityId;
        return meshTransformData;
    }

    ShaderProgramData BuildShaderProgramData(
        SHADER_RID shaderProgramId,
        const ShaderAsset* vertexShader,
        const ShaderAsset* fragmentShader)
    {
        ShaderProgramData shaderProgramData;
        shaderProgramData.id = shaderProgramId;
        shaderProgramData.vertexShaderId = vertexShader->id;
        shaderProgramData.fragmentShaderId = fragmentShader->id;
        shaderProgramData.vertexShader = vertexShader;
        shaderProgramData.fragmentShader = fragmentShader;
        return shaderProgramData;
    }
}


Renderer::Renderer(AssetManager& assetManager)
    : device(std::make_unique<OpenGLDevice>())
    , assetManagerRef(assetManager) {
    // auto select OPENGL for now
}

Renderer::~Renderer() {
    if (device) {
        for (auto& [shaderProgramId, shaderProgram] : renderData.shaderPrograms) {
            if (shaderProgram.deviceProgramId != INVALID_SPDEVICE_RID) {
                device->DestroyShaderProgram(shaderProgram.deviceProgramId);
            }
        }
        device->Close();
    }
}

void Renderer::ConfigureWindow() {
    device->ConfigureWindow();
}

bool Renderer::Init(WindowManager& window) {
    return device->Init(window);
}

void Renderer::BeginFrame() {
    device->BeginFrame();
}

void Renderer::SubmitMesh(UUID meshId, ECS_RID entityId, Matrix4 matrix) {
    
    // build mesh render data if it doesn't exist
    if (renderData.meshRenderData.find(meshId) == renderData.meshRenderData.end()) {
        const MeshAsset * meshAsset = assetManagerRef.RequestAssetReadOnly<MeshAsset>(meshId);
        renderData.meshRenderData[meshId] = BuildMeshRenderData(meshAsset);
        for (int i = 0; i < meshAsset->primitives.size(); ++i) {
            renderData.meshRenderData[meshId].primitives.push_back(BuildPrimitiveRenderData(meshAsset, i));
        }
    }

    // build mesh transform if it doesn't exist
    if (renderData.meshTransformData.find(entityId) == renderData.meshTransformData.end()) {
        MeshTransformData transformData = BuildMeshTransformData(&renderData.meshRenderData[meshId], entityId);
        renderData.meshTransformData[entityId] = transformData;
    }
    // always update the transform matrix for the entity
    renderData.meshTransformData[entityId].worldTransform = matrix;
}

void Renderer::Render() {
    device->Render(renderData);
}

void Renderer::EndFrame() {
    device->EndFrame();
}

/**
 * @brief 
 * 
 * @param definition 
 * @return SH_RID 
 */
SHADER_RID Renderer::RegisterShaderProgram(UUID vertexShaderId, UUID fragmentShaderId) {
    if (vertexShaderId == 0 || fragmentShaderId == 0) {
        throw std::runtime_error("A shader program requires both vertex and fragment shader assets.");
    }

    // return if already exists in map
    for (const auto& [shaderProgramId, runtimeData] : renderData.shaderPrograms) {
        if (runtimeData.vertexShaderId == vertexShaderId
            && runtimeData.fragmentShaderId == fragmentShaderId) {
            return shaderProgramId;
        }
    }

    // retrieve info to build shader data
    SHADER_RID shaderProgramId = nextShaderProgramId++;
    const ShaderAsset* vertexShader = assetManagerRef.RequestAssetReadOnly<ShaderAsset>(
        vertexShaderId
    );
    const ShaderAsset* fragmentShader = assetManagerRef.RequestAssetReadOnly<ShaderAsset>(
        fragmentShaderId
    );
    if (!vertexShader || !fragmentShader) {
        throw std::runtime_error("Failed to load vertex or fragment shader asset.");
    }

    // build shader data and shader program
    ShaderProgramData runtimeData = BuildShaderProgramData(
        shaderProgramId,
        vertexShader,
        fragmentShader
    );
    SPDEVICE_RID deviceProgramId = device->CreateShaderProgram(
        runtimeData.vertexShader->shaderSource,
        runtimeData.fragmentShader->shaderSource
    );
    runtimeData.deviceProgramId = deviceProgramId;
    
    // register in map
    renderData.shaderPrograms[runtimeData.id] = runtimeData;
    return runtimeData.id;
}

void Renderer::SetDefaultShaderProgram(SHADER_RID shaderProgramId) {
    if (!HasShaderProgram(shaderProgramId)) {
        Logger::Error("Renderer", "Cannot set an unregistered shader program as the default.");
        return;
    }
    defaultShaderProgram = shaderProgramId;
}

bool Renderer::HasShaderProgram(SHADER_RID shaderProgramId) const {
    return shaderProgramId != INVALID_SH_RID
        && renderData.shaderPrograms.find(shaderProgramId) != renderData.shaderPrograms.end();
}