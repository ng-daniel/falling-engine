#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "assetManagement/textureLoader.h"

TextureData TextureLoader::LoadTexture(const std::string & filePath) {
    TextureData textureData;
    textureData.data = stbi_load(filePath.c_str(), &textureData.width, &textureData.height, &textureData.channels, 0);
    if (textureData.data == nullptr) {
        std::cout << "Failed to load texture: " << filePath << std::endl;
    }
    return textureData;
}