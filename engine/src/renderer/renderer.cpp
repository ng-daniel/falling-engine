#include "engine/renderer/renderer.h"
#include "engine/renderer/opengl/opengl_device.h"

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

void Renderer::SubmitMesh(UUID meshId, const Matrix4& transform) {
    if (renderData.meshRenderData.find(meshId) == renderData.meshRenderData.end()) {
        // instantiate new mesh render data if doesn't exist
        MeshRenderData meshRenderData;
        meshRenderData.meshId = meshId;
        meshRenderData.mesh = assetManagerRef.RequestAssetReadOnly<MeshAsset>(meshId);
        meshRenderData.deviceData = nullptr;
        renderData.meshRenderData[meshId] = meshRenderData;
    }
    renderData.meshTransformData[meshId] = { &renderData.meshRenderData[meshId], transform };
}

void Renderer::Render() {
    device->Render(renderData);
}

void Renderer::EndFrame() {
    device->EndFrame();
}
