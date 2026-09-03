#include "engine/renderer/renderer.h"
#include "engine/renderer/opengl/opengl_device.h"
#include "engine/renderer/renderer_structures.h"

namespace {
    PrimitiveRenderData BuildPrimitiveRenderData(const MeshAsset * parentMesh, int pIdx) {
        PrimitiveRenderData primitiveRenderData;
        primitiveRenderData.mesh = parentMesh;
        primitiveRenderData.pIdx = pIdx;
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
}


Renderer::Renderer(AssetManager& assetManager)
    : device(std::make_unique<OpenGLDevice>())
    , assetManagerRef(assetManager) {
    // auto select OPENGL for now
}

Renderer::~Renderer() {
    if (device) {
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
    renderData.meshTransformData[entityId].transform = matrix;
}

void Renderer::Render() {
    device->Render(renderData);
}

void Renderer::EndFrame() {
    device->EndFrame();
}
