#include <iostream>
#include <cmath>
#include <string>

#include "include/generated/asset_ids.h"
#include "engine/core/application.h"

int main() {
    Application app = Application("./game/assets");
    AssetManager& assetManager = app.GetAssetManager();

    // TESTING

    const ImageAsset * img = assetManager.RequestAssetReadOnly<ImageAsset>(GameAssets::RYUJINTEX_IMAGE); // Example usage of RequestAsset
    std::cout << "Finished loading image asset with ID: " << GameAssets::RYUJINTEX_IMAGE.GetUUID() << std::endl;
    std::cout << "Image Asset Size: " << img->data.size() << std::endl;

    const ImageAsset * img2 = assetManager.RequestAssetReadOnly<ImageAsset>(GameAssets::RYUJINTEXPNG_IMAGE); // Example usage of RequestAsset
    std::cout << "Finished loading image asset with ID: " << GameAssets::RYUJINTEXPNG_IMAGE.GetUUID() << std::endl;
    std::cout << "Image Asset Size: " << img2->data.size() << std::endl;

    const ShaderAsset * shader = assetManager.RequestAssetReadOnly<ShaderAsset>(GameAssets::BASICFRAG_SHADER); // Example usage of RequestAsset
    std::cout << "Shader Asset Source: " << shader->shaderSource << std::endl;

    return 0;
}
