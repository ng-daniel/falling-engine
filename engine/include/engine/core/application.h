#pragma once

#include <filesystem>
#include "engine/input/input_manager.h"
#include "engine/renderer/renderer.h"
#include "engine/core/window_manager.h"
#include "engine/assets/asset_manager.h"
#include "engine/ecs/ecs_manager.h"
#include "engine/scripting/script_manager.h"
#include "engine/debug/profiler.h"
#include "engine/physics/physics_manager.h"

/**
 * @brief Main application class that manages the game loop and overall application state.
 */
class Application {
public:
    Application(std::filesystem::path assetRoot);
    ~Application();

    void Run();

    AssetManager& GetAssetManager() { return assetManager; }
    EcsManager& GetECSManager() { return ecsManager; }
    ScriptManager& GetScriptSystem() { return scriptSystem; }
    Renderer& GetRenderer() { return renderer; }
    InputManager& GetInputManager() { return inputManager; }
    Profiler& GetProfiler() { return profiler; }
    PhysicsManager& GetPhysicsManager() { return physicsManager; }
private:
    InputManager inputManager;
    WindowManager window;
    AssetManager assetManager;
    EcsManager ecsManager;
    ScriptManager scriptSystem;
    Renderer renderer;
    Profiler profiler;
    PhysicsManager physicsManager;
};
