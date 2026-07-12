#include <iostream>

#include "engine/core/application.h"
#include "engine/utils/uuid.h"

Application::Application(std::filesystem::path assetRoot) 
    : assetManager(std::move(assetRoot)) {
    std::cout << "Application Initialized!" << std::endl;

    // TESTING

    UUID textureAssetID = 2855671031843873317; // Example UUID for a TextureAsset
    const TextureAsset * tex = assetManager.RequestAssetReadOnly<TextureAsset>(textureAssetID); // Example usage of RequestAsset
    std::cout << "Finished loading texture asset with ID: " << textureAssetID << std::endl;
    std::cout << "Texture Asset Size: " << tex->data.size() << std::endl;


    UUID shaderAssetID = 7602981331776155094; // Example UUID for a ShaderAsset
    const ShaderAsset * shader = assetManager.RequestAssetReadOnly<ShaderAsset>(shaderAssetID); // Example usage of RequestAsset
    std::cout << "Shader Asset Source: " << shader->shaderSource << std::endl;

    return;
}

Application::~Application() {
    return;
}

void Application::Run() {
    return;
}