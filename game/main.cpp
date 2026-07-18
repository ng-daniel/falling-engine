#include <iostream>
#include <cmath>
#include <string>

#include "include/generated/asset_ids.h"
#include "engine/core/application.h"

int main() {
    Application app = Application("./game/assets");
    AssetManager& assetManager = app.GetAssetManager();

    // TESTING

    UUID imageAssetID = GameAssets::LOOKALIKE_CROP_IMAGE.GetUUID();
    const ImageAsset * img = assetManager.RequestAssetReadOnly<ImageAsset>(imageAssetID); // Example usage of RequestAsset
    std::cout << "Finished loading image asset with ID: " << imageAssetID << std::endl;
    std::cout << "Image Asset Size: " << img->data.size() << std::endl;


    UUID shaderAssetID = GameAssets::BASICFRAG_SHADER.GetUUID(); // Example UUID for a ShaderAsset
    const ShaderAsset * shader = assetManager.RequestAssetReadOnly<ShaderAsset>(shaderAssetID); // Example usage of RequestAsset
    std::cout << "Shader Asset Source: " << shader->shaderSource << std::endl;

    return 0;
}
