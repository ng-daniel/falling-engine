#include <iostream>
#include <cmath>

#include "include/generated/asset_ids.h"
#include "engine/core/application.h"

int main() {
    Application app = Application("./game/assets");
    AssetManager& assetManager = app.GetAssetManager();

    // TESTING
    const ShaderAsset * shader = assetManager.RequestAssetReadOnly<ShaderAsset>(GameAssets::BASICFRAG_SHADER); // Example usage of RequestAsset
    std::cout << "Shader Asset Source: " << shader->shaderSource << std::endl;

    const ImageAsset * img3 = assetManager.RequestAssetReadOnly<ImageAsset>(GameAssets::LOOKALIKE_CROP_IMAGE); // Example usage of RequestAsset
    std::cout << "Finished loading image asset with ID: " << GameAssets::LOOKALIKE_CROP_IMAGE.GetUUID() << std::endl;
    std::cout << "Image Asset Size: " << img3->data.size() << std::endl;

    const ImageAsset * img = assetManager.RequestAssetReadOnly<ImageAsset>(GameAssets::RYUJINTEX_IMAGE); // Example usage of RequestAsset
    std::cout << "Finished loading image asset with ID: " << GameAssets::RYUJINTEX_IMAGE.GetUUID() << std::endl;
    std::cout << "Image Asset Size: " << img->data.size() << std::endl;

    const ImageAsset * img2 = assetManager.RequestAssetReadOnly<ImageAsset>(GameAssets::IMAGE__RYUJINTEX_IMAGE); // Example usage of RequestAsset
    std::cout << "Finished loading image asset with ID: " << GameAssets::IMAGE__RYUJINTEX_IMAGE.GetUUID() << std::endl;
    std::cout << "Image Asset Size: " << img2->data.size() << std::endl;

    const TextureAsset * tex = assetManager.RequestAssetReadOnly<TextureAsset>(GameAssets::TEXTURE__RYUJINTEX_TEXTURE); // Example usage of RequestAsset
    std::cout << "Finished loading texture asset with ID: " << GameAssets::TEXTURE__RYUJINTEX_TEXTURE.GetUUID() << std::endl;
    std::cout << "Texture Asset Properties: " 
              << "mag_filter=" << static_cast<int>(tex->mag_filter) 
              << ", min_filter=" << static_cast<int>(tex->min_filter) 
              << ", wrap_s=" << static_cast<int>(tex->wrap_s) 
              << ", wrap_t=" << static_cast<int>(tex->wrap_t) 
              << std::endl;

    return 0;
}
