#include <iostream>
#include <cmath>

// #include "include/generated/asset_ids.h"
#include "engine/core/application.h"

int main() {
    Application app = Application("./game/assets");
    AssetManager& assetManager = app.GetAssetManager();

    // // TESTING
    // const ShaderAsset * shader = assetManager.RequestAssetReadOnly<ShaderAsset>(GameAssets::BASICFRAG_SHADER); // Example usage of RequestAsset
    // std::cout << "Shader Asset Source: " << shader->shaderSource << std::endl;

    // const TextureAsset * tex = assetManager.RequestAssetReadOnly<TextureAsset>(GameAssets::RYUJIN7_RYUJINTEX_TEXTURE); // Example usage of RequestAsset
    // std::cout << "Finished loading texture asset with ID: " << GameAssets::RYUJIN7_RYUJINTEX_TEXTURE.GetUUID() << std::endl;
    // std::cout << "Texture Asset Properties: " 
    //           << "mag_filter=" << static_cast<int>(tex->mag_filter) 
    //           << ", min_filter=" << static_cast<int>(tex->min_filter) 
    //           << ", wrap_s=" << static_cast<int>(tex->wrap_s) 
    //           << ", wrap_t=" << static_cast<int>(tex->wrap_t) 
    //           << std::endl;

    return 0;
}
