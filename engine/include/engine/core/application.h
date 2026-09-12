#pragma once

#include <filesystem>
#include "engine/input/input_manager.h"
#include "engine/renderer/renderer.h"
#include "engine/core/window_manager.h"
#include "engine/assets/asset_manager.h"
#include "engine/ecs/ecs_manager.h"

/// @brief Main application class that manages the game loop and overall application state.
class Application {
public:
    Application(std::filesystem::path assetRoot);
    ~Application();

    void Run();

    AssetManager& GetAssetManager() { return assetManager; }
    EcsManager& GetECSManager() { return ecsManager; }
    Renderer& GetRenderer() { return renderer; }
    InputManager& GetInputManager() { return inputManager; }
private:
    WindowManager window;
    AssetManager assetManager;
    EcsManager ecsManager;
    Renderer renderer;
    InputManager inputManager;
};
