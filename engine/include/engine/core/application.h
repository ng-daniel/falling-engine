#ifndef ENGINE_CORE_APPLICATION_H
#define ENGINE_CORE_APPLICATION_H

#include <filesystem>

#include "engine/assets/asset_manager.h"

/// @brief Main application class that manages the game loop and overall application state.
class Application {
public:
    Application(std::filesystem::path assetRoot);
    ~Application();

    void Run();
    AssetManager& GetAssetManager() { return assetManager; }

private:
    AssetManager assetManager;
};

#endif // ENGINE_CORE_APPLICATION_H