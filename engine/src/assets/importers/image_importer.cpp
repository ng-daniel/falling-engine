#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>
#include "engine/assets/importers/image_importer.h"
#include "engine/assets/asset_helpers.h"

/**
 * @brief Loads an image asset from the given file path.
 * 
 * @param path The file path to the image asset.
 * @return std::vector<std::unique_ptr<Asset>> The loaded image asset.
 */
std::vector<std::unique_ptr<Asset>> ImageImporter::LoadAsset(SourceAssetMetadata& metadata) {
    int width, height, numChannels;
    unsigned char * data = stbi_load(metadata.path.string().c_str(), &width, &height, &numChannels, 0);
    if (!data) {
        throw std::runtime_error("Failed to load image: " + metadata.path.string());
    }

    std::unique_ptr<ImageAsset> imageAsset = std::make_unique<ImageAsset>();
    imageAsset->width = width;
    imageAsset->height = height;
    imageAsset->numChannels = numChannels;
    size_t dataSize = width * height * numChannels;
    imageAsset->data.assign(data, data + dataSize);

    ApplyMetadataToAsset(
        metadata.id, 
        metadata.path.stem().string(), 
        "Image", 
        *imageAsset
    );

    std::vector<std::unique_ptr<Asset>> assets;
    assets.push_back(std::move(imageAsset));
    stbi_image_free(data);

    return assets;
}

/**
 * @brief Loads an image asset from the given memory buffer.
 * 
 * @param data The memory buffer containing the image data.
 * @return std::vector<std::unique_ptr<Asset>> The loaded image asset.
 */
std::vector<std::unique_ptr<Asset>> ImageImporter::LoadAssetFromMemory(const std::vector<unsigned char>& data) {
    int width, height, numChannels;
    unsigned char * imageData = stbi_load_from_memory(data.data(), data.size(), &width, &height, &numChannels, 0);
    if (!imageData) {
        throw std::runtime_error("Failed to load image from memory");
    }

    std::unique_ptr<ImageAsset> imageAsset = std::make_unique<ImageAsset>();
    imageAsset->width = width;
    imageAsset->height = height;
    imageAsset->numChannels = numChannels;
    size_t dataSize = width * height * numChannels;
    imageAsset->data.assign(imageData, imageData + dataSize);

    std::vector<std::unique_ptr<Asset>> assets;
    assets.push_back(std::move(imageAsset));

    stbi_image_free(imageData);

    return assets;
}