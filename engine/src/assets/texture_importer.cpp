#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "engine/assets/texture_importer.h"

/**
 * @brief Loads a texture asset from the given file path.
 * 
 * @param path The file path to the texture asset.
 * @return Asset The loaded texture asset.
 */
std::unique_ptr<Asset> TextureImporter::LoadAsset(const std::filesystem::path& path) {
    int width, height, numChannels;
    unsigned char * data = stbi_load(path.string().c_str(), &width, &height, &numChannels, 0);
    if (!data) {
        throw std::runtime_error("Failed to load texture: " + path.string());
    }

    std::unique_ptr<TextureAsset> textureAsset = std::make_unique<TextureAsset>();
    textureAsset->width = width;
    textureAsset->height = height;
    textureAsset->numChannels = numChannels;

    size_t dataSize = width * height * numChannels;
    textureAsset->data.assign(data, data + dataSize);

    stbi_image_free(data);

    return textureAsset;
}