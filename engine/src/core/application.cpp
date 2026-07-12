#include <iostream>

#include "engine/core/application.h"
#include "engine/assets/asset_data.h"
#include "engine/utils/uuid.h"

Application::Application(std::filesystem::path assetRoot) 
    : assetManager(std::move(assetRoot)) {
    std::cout << "Application Initialized!" << std::endl;

    // TESTING

    UUID imageAssetID = 2855671031843873317; // Example UUID for a TextureAsset
    const ImageAsset * img = assetManager.RequestAssetReadOnly<ImageAsset>(imageAssetID); // Example usage of RequestAsset
    std::cout << "Finished loading image asset with ID: " << imageAssetID << std::endl;
    std::cout << "Image Asset Size: " << img->data.size() << std::endl;


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