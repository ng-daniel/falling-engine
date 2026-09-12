#include "engine/core/application.h"
#include "engine/core/window_manager.h"
#include "engine/debug/logger.h"
#include "engine/ecs/components/mesh_renderer.h"
#include "engine/input/input_manager.h"
#include "engine/utils/random.h"
#include "engine/utils/time.h"

#include <stdexcept>
#include <unordered_set>

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

    // 4. initialize input manager with glfw window
    if (!inputManager.Init(window)) {
        throw std::runtime_error("Failed to initialize the input manager");
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
        inputManager.Update();

        renderer.BeginFrame(inputManager);
        
        std::unordered_set<ECS_RID> rotatedEntities;
        float rotationSpeed = 0.5f;
        
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
            // rotate the mesh parent around the Y-axis over time
            
            Entity* parentEntity = ecsManager.GetParent(*entity);
            if (parentEntity && rotatedEntities.find(parentEntity->entityRuntimeIdx) == rotatedEntities.end()) {
                Transform* parentTransform = ecsManager.GetComponent<Transform>(*parentEntity);
                if (parentTransform) {
                    rotationSpeed = Random::RandFloat(2.0f, 2.5f);
                    parentTransform->ChangeRotation(*parentTransform, Quaternion::EulerToQuaternion(0.0f, rotationSpeed, 0.0f));
                    rotatedEntities.insert(parentEntity->entityRuntimeIdx);
                }
            }
            
            renderer.SubmitMesh(
                meshRenderer.meshId,
                ecsManager.ComputeWorldTransform(*entity).matrix
            );
        }
        renderer.Render();
        renderer.EndFrame();

        window.EndFrame();
    }
}
