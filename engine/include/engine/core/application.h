#pragma once

#include <filesystem>

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
private:
    AssetManager assetManager;
    EcsManager ecsManager;
};
