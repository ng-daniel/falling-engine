#include <iostream>

#include "engine/core/application.h"

Application::Application(std::filesystem::path assetRoot) 
    : assetManager(std::move(assetRoot)) {
    std::cout << "Application Initialized!" << std::endl;
    return;
}

Application::~Application() {
    return;
}

void Application::Run() {
    return;
}