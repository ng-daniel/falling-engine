#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>
#include "engine/assets/texture_importer.h"
#include "engine/assets/asset_helpers.h"

/**
 * @brief Loads a texture asset from the given file path.
 * 
 * @param path The file path to the texture asset.
 * @return std::vector<std::unique_ptr<Asset>> The loaded texture asset.
 */
std::vector<std::unique_ptr<Asset>> TextureImporter::LoadAsset(AssetMetadata& metadata) {
    int width, height, numChannels;
    unsigned char * data = stbi_load(metadata.path.string().c_str(), &width, &height, &numChannels, 0);
    if (!data) {
        throw std::runtime_error("Failed to load texture: " + metadata.path.string());
    }

    std::unique_ptr<TextureAsset> textureAsset = std::make_unique<TextureAsset>();
    textureAsset->width = width;
    textureAsset->height = height;
    textureAsset->numChannels = numChannels;
    size_t dataSize = width * height * numChannels;
    textureAsset->data.assign(data, data + dataSize);

    ApplyMetadataToAsset(
        metadata.id, 
        metadata.path.stem().string(), 
        "Texture", 
        *textureAsset
    );

    std::vector<std::unique_ptr<Asset>> assets;
    assets.push_back(std::move(textureAsset));
    stbi_image_free(data);

    return assets;
}

/**
 * @brief Loads a texture asset from the given memory buffer.
 * 
 * @param data The memory buffer containing the texture data.
 * @return std::vector<std::unique_ptr<Asset>> The loaded texture asset.
 */
std::vector<std::unique_ptr<Asset>> TextureImporter::LoadAssetFromMemory(const std::vector<unsigned char>& data) {
    int width, height, numChannels;
    unsigned char * textureData = stbi_load_from_memory(data.data(), data.size(), &width, &height, &numChannels, 0);
    if (!textureData) {
        throw std::runtime_error("Failed to load texture from memory");
    }

    std::unique_ptr<TextureAsset> textureAsset = std::make_unique<TextureAsset>();
    textureAsset->width = width;
    textureAsset->height = height;
    textureAsset->numChannels = numChannels;
    size_t dataSize = width * height * numChannels;
    textureAsset->data.assign(textureData, textureData + dataSize);

    std::vector<std::unique_ptr<Asset>> assets;
    assets.push_back(std::move(textureAsset));

    stbi_image_free(textureData);

    return assets;
}