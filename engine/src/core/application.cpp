#include <iostream>

#include "engine/core/application.h"
#include "engine/utils/uuid.h"

Application::Application(std::filesystem::path assetRoot) 
    : assetManager(std::move(assetRoot)) {
    std::cout << "Application Initialized!" << std::endl;

    // TESTING

    UUID textureAssetID = 12551888299912889363; // Example UUID for a TextureAsset
    TextureAsset * tex = assetManager.RequestAsset<TextureAsset>(textureAssetID); // Example usage of RequestAsset
    std::cout << "Texture Asset Size: " << tex->data.size() << std::endl;


    UUID shaderAssetID = 7075911361046789303; // Example UUID for a ShaderAsset
    ShaderAsset * shader = assetManager.RequestAsset<ShaderAsset>(shaderAssetID); // Example usage of RequestAsset
    std::cout << "Shader Asset Source: " << shader->shaderSource << std::endl;

    return;
}

Application::~Application() {
    return;
}

void Application::Run() {
    return;
}