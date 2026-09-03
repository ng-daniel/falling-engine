#include "engine/core/application.h"
#include "engine/debug/logger.h"
#include "engine/ecs/components/mesh_renderer.h"
#include <stdexcept>

Application::Application(std::filesystem::path assetRoot) 
    : assetManager(assetRoot)
    , renderer(assetManager)
    {
    // 1. assetManager and ECS are initialized first

    // 2. initialize the window and apply target graphics configuration
    if (!window.Init([this]() { renderer.ConfigureWindow(); })) {
        throw std::runtime_error("Failed to initialize the application window");
    }

    // 3. initialize the renderer after its graphics context exists
    if (!renderer.Init(window)) {
        throw std::runtime_error("Failed to initialize the renderer");
    }
    Logger::Info("Application", "Application initialized.");
}

Application::~Application() = default;

void Application::Run() {
    while (!window.ShouldClose()) {
        window.BeginFrame();

        renderer.BeginFrame();
        EntityComponentView<MeshRenderer> meshRenderers = ecsManager.GetEntityComponentView<MeshRenderer>();
        for (auto [entityRuntimeId, meshRenderer] : meshRenderers) {
            Entity * entity = ecsManager.GetEntity(entityRuntimeId);
            if (!entity) {
                Logger::Warning("Application", "Entity not found for runtime ID: " + std::to_string(entityRuntimeId));
                continue;
            }
            Transform * transform = ecsManager.GetComponent<Transform>(*entity);
            if (!transform) {
                Logger::Warning("Application", "Transform component not found for entity with runtime ID: " + std::to_string(entityRuntimeId));
                continue;
            }
            renderer.SubmitMesh(meshRenderer.meshId, transform->matrix);
        }
        renderer.EndFrame();

        window.EndFrame();
    }
}
