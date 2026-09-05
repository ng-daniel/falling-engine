#include "engine/renderer/renderer.h"
#include "engine/renderer/opengl/opengl_device.h"
#include "engine/renderer/renderer_structures.h"

#include "engine/debug/logger.h"

#include <stdexcept>

namespace {
    PrimitiveRenderData BuildPrimitiveRenderData(const MeshAsset * parentMesh, int pIdx) {
        PrimitiveRenderData primitiveRenderData;
        primitiveRenderData.meshId = parentMesh->id;
        primitiveRenderData.pIdx = pIdx;
        primitiveRenderData.materialId = parentMesh->primitives[pIdx].material;
        primitiveRenderData.graphicsDeviceData = nullptr;
        return primitiveRenderData;
    }
    
    MeshRenderData BuildMeshRenderData(const MeshAsset * meshAsset) {
        MeshRenderData meshRenderData;
        meshRenderData.meshId = meshAsset->id;
        meshRenderData.meshId = meshAsset->id;
        return meshRenderData;
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
    if (!meshRenderData->initialized) {
        device->InitializeGPUBuffersForMesh(*meshRenderData, assetManagerRef.RequestAssetReadOnly<MeshAsset>(meshId));
        meshRenderData->initialized = true;
    }
    frameData.submissions.push_back({ meshRenderData, worldTransform });
}

void Renderer::Render() {
    device->Render(frameData);
}

void Renderer::EndFrame() {
    device->EndFrame();
}

SHADER_RID Renderer::RegisterShaderProgram(const ShaderProgramData& shaderProgram) {
    if (shaderProgram.id == INVALID_SH_RID
        || shaderProgram.vertexShaderId == 0
        || shaderProgram.fragmentShaderId == 0) {
        throw std::invalid_argument("A shader program requires an ID and both shader assets.");
    }

    // check if the shader program is already cached
    const auto cachedProgram = shaderCache.find(shaderProgram.id);
    if (cachedProgram != shaderCache.end()) {
        if (cachedProgram->second.vertexShaderId != shaderProgram.vertexShaderId
            || cachedProgram->second.fragmentShaderId != shaderProgram.fragmentShaderId) {
            throw std::invalid_argument("A different shader program already uses this UUID.");
        }
        return cachedProgram->first;
    }

    // load the shader assets
    const ShaderAsset* vertexShader = assetManagerRef.RequestAssetReadOnly<ShaderAsset>(
        shaderProgram.vertexShaderId
    );
    const ShaderAsset* fragmentShader = assetManagerRef.RequestAssetReadOnly<ShaderAsset>(
        shaderProgram.fragmentShaderId
    );
    if (!vertexShader || !fragmentShader) {
        throw std::runtime_error("Failed to load vertex or fragment shader asset.");
    }
    if (vertexShader->shaderType != ShaderAsset::ShaderType::Vertex
        || fragmentShader->shaderType != ShaderAsset::ShaderType::Fragment) {
        throw std::invalid_argument("Shader assets do not match their requested stages.");
    }

    // create the shader program on the graphics device
    ShaderProgramData runtimeProgram = shaderProgram;
    runtimeProgram.deviceProgramId = device->CreateShaderProgram(
        vertexShader->shaderSource,
        fragmentShader->shaderSource
    );
    if (runtimeProgram.deviceProgramId == INVALID_SPDEVICE_RID) {
        throw std::runtime_error("The graphics device failed to create the shader program.");
    }

    // put generated program into cache
    shaderCache.emplace(runtimeProgram.id, runtimeProgram);
    return runtimeProgram.id;
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
