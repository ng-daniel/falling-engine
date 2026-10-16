#include "engine/core/application.h"
#include "engine/core/window_manager.h"
#include "engine/debug/logger.h"
#include "engine/renderer/components/mesh_renderer.h"
#include "engine/input/input_manager.h"
#include "engine/utils/time.h"

#include <stdexcept>

Application::Application(std::filesystem::path assetRoot) 
    : assetManager(assetRoot)
    , scriptSystem(ecsManager)
    , renderer(assetManager)
    {
    // 1. assetManager and ECS are initialized first

    // 2. initialize the window and apply target graphics configuration
    if (!window.Init(inputManager, [this]() { renderer.ConfigureWindow(); })) {
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
    Time::SetFpsTrackingInterval(1.0f);
    Time::ToggleFPSTracking(true);
    Time::ToggleFPSLogging(true);
    Time::Reset();

    while (!window.ShouldClose()) {
        Time::Update();
        
        profiler.BeginFrame();

        {
            ProfileScope scope(profiler, "Input Update");
            inputManager.Update();
        }
        
        {
            ProfileScope scope(profiler, "Script Update");
            scriptSystem.UpdateAllScripts();
        }

        renderer.BeginFrame(inputManager);
        {
            ProfileScope scope(profiler, "Mesh Submission");            

            EntityComponentView<MeshRenderer> meshRenderers = ecsManager.GetEntityComponentView<MeshRenderer>();
            for (auto [entityRuntimeId, meshRenderer] : meshRenderers) {
                Entity * entity = ecsManager.GetEntity(entityRuntimeId);
                if (!entity) {
                    Logger::Warning("Application", "Entity not found for runtime ID: " + std::to_string(entityRuntimeId));
                    continue;
                }
                renderer.SubmitMesh(
                    meshRenderer.meshId,
                    ecsManager.ComputeWorldTransform(*entity).matrix
                );
            }
        }
        
        {
            ProfileScope scope(profiler, "Render");
            renderer.Render();
        }
        renderer.EndFrame();

        ecsManager.FlushRemovals();

        profiler.EndFrame();

        window.EndFrame();

        profiler.LogLastFrame();
    }
}
