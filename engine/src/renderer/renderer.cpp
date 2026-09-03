#include "engine/renderer/renderer.h"

#include "engine/renderer/opengl/opengl_device.h"

Renderer::Renderer()
    : device(std::make_unique<OpenGLDevice>()) {
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

bool Renderer::Init(Window& window) {
    return device->Init(window);
}

void Renderer::BeginFrame() {
    device->BeginFrame();
}

void Renderer::Submit(UUID mesh) {
    device->Submit(mesh);
}

void Renderer::Render() {
    device->Render();
}

void Renderer::EndFrame() {
    device->EndFrame();
}
