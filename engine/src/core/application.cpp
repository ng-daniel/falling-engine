#include "engine/core/application.h"
#include "engine/debug/logger.h"

Application::Application(std::filesystem::path assetRoot) 
    : assetManager(assetRoot) {
    Logger::Info("Application", "Application initialized.");
}

Application::~Application() {
    return;
}

void Application::Run() {
    while (!window.ShouldClose()) {
        window.BeginFrame();

        

        window.EndFrame();
    }
}