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
    : assetManagerRef(assetManager)
    , device(std::make_unique<OpenGLDevice>()) {
    // auto select OPENGL for now
}

Renderer::~Renderer() {
    if (device) {
        for (auto& [shaderProgramId, shaderProgram] : shaderCache) {
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
    frameData.submissions.clear();
    device->BeginFrame();
}

void Renderer::SubmitMesh(UUID meshId, const Matrix4& worldTransform) {
    MeshRenderData* meshRenderData = GetOrCreateMeshRenderData(meshId);
    if (!meshRenderData) {
        return;
    }

    frameData.submissions.push_back({ meshRenderData, worldTransform });
}

void Renderer::Render() {
    device->Render(frameData);
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
    for (const auto& [shaderProgramId, runtimeData] : shaderCache) {
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
    shaderCache[runtimeData.id] = runtimeData;
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
        && shaderCache.find(shaderProgramId) != shaderCache.end();
}

MeshRenderData* Renderer::GetOrCreateMeshRenderData(UUID meshId) {
    const auto cachedMeshIt = meshCache.find(meshId);
    if (cachedMeshIt != meshCache.end()) {
        return &cachedMeshIt->second;
    }

    const MeshAsset* meshAsset = assetManagerRef.RequestAssetReadOnly<MeshAsset>(meshId);
    if (!meshAsset) {
        Logger::Error("Renderer", "Failed to load mesh asset " + std::to_string(meshId) + ".");
        return nullptr;
    }

    MeshRenderData meshRenderData = BuildMeshRenderData(meshAsset);
    meshRenderData.primitives.reserve(meshAsset->primitives.size());
    for (std::size_t primitiveIndex = 0; primitiveIndex < meshAsset->primitives.size(); ++primitiveIndex) {
        meshRenderData.primitives.push_back(
            BuildPrimitiveRenderData(meshAsset, static_cast<int>(primitiveIndex))
        );
    }

    const auto insertionResult = meshCache.emplace(meshId, std::move(meshRenderData));
    return &insertionResult.first->second;
}
