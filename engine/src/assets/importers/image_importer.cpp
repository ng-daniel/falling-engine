#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string>
#include <vector>

#include "engine/assets/asset_warehouse_service.h"
#include "engine/assets/importers/image_importer.h"
#include "engine/assets/asset_helpers.h"

/**
 * @brief Loads an image asset from the given file path.
 * 
 * @param path The file path to the image asset.
 * @return const ImageAsset* The stored image asset.
 */
const ImageAsset* ImageImporter::LoadAsset(
    SourceAssetMetadata& metadata,
    AssetWarehouseService& assetWarehouseService
) {
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
        metadata,
        *imageAsset
    );
    stbi_image_free(data);

    return static_cast<const ImageAsset*>(
        assetWarehouseService.StoreAsset(metadata, std::move(imageAsset))
    );
}

/**
 * @brief Loads an image asset from the given memory buffer.
 * 
 * @param data The memory buffer containing the image data.
 * @return const ImageAsset* The stored image asset.
 */
const ImageAsset* ImageImporter::LoadAssetFromMemory(
    SourceAssetMetadata& metadata,
    AssetWarehouseService& assetWarehouseService,
    const std::vector<unsigned char>& data,
    const std::string& subAssetIdentifier,
    RuntimeAssetMetadata* runtimeMetadata
) {
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

    if (runtimeMetadata != nullptr) {
        ApplyMetadataToAsset(*runtimeMetadata, *imageAsset);
    } else {
        imageAsset->name = subAssetIdentifier;
        imageAsset->type = Asset::AssetType::Image;
    }

    stbi_image_free(imageData);

    return static_cast<const ImageAsset*>(
        assetWarehouseService.StoreAsset(metadata, std::move(imageAsset))
    );
}