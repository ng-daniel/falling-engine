#include "engine/core/application.h"
#include "engine/debug/logger.h"
#include <stdexcept>

Application::Application(std::filesystem::path assetRoot) 
    : assetManager(assetRoot) {
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

        

        window.EndFrame();
    }
}
