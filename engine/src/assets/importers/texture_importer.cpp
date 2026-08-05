#include "engine/assets/importers/texture_importer.h"

#include <iostream>
#include <memory>
#include <stdexcept>

#include "engine/assets/asset_helpers.h"
#include "engine/assets/asset_warehouse_service.h"
#include "engine/assets/importers/image_importer.h"

const std::string TEX_SUBASSET_IDENTIFIER_SUFFIX = "_texture";

std::string BuildTexSubAssetIdentifier(const std::string& imageSubAssetIdentifier) {
    return imageSubAssetIdentifier + TEX_SUBASSET_IDENTIFIER_SUFFIX;
}

const TextureAsset* TextureImporter::LoadAsset(
    SourceAssetMetadata& metadata,
    AssetWarehouseService& assetWarehouseService
) {
    const ImageAsset* imageAsset = ImageImporter::LoadAsset(metadata, assetWarehouseService);
    RuntimeAssetMetadata* runtimeMetadata = assetWarehouseService.FindRuntimeMetadata(imageAsset->id);
    std::cout << "Loaded image asset with ID: " << imageAsset->id << " with name: " << runtimeMetadata->subAssetIdentifier << std::endl;

    return LoadAssetFromImage(
        metadata,
        assetWarehouseService,
        imageAsset->id,
        BuildTexSubAssetIdentifier(runtimeMetadata->subAssetIdentifier)
    );
}

const TextureAsset* TextureImporter::LoadAssetFromImage(
    SourceAssetMetadata& metadata,
    AssetWarehouseService& assetWarehouseService,
    UUID imageId,
    const std::string& subAssetIdentifier
) {
    const Asset* loadedAsset = assetWarehouseService.GetLoadedAssetReadOnly(imageId);
    if (loadedAsset == nullptr) {
        throw std::runtime_error("Texture image dependency is not loaded in the warehouse.");
    }
    if (loadedAsset->type != Asset::AssetType::Image) {
        throw std::runtime_error(
            "Texture image dependency is not an Image asset, instead it is of type: "
             + GetStringFromAssetType(loadedAsset->type)
        );
    }

    auto textureAsset = std::make_unique<TextureAsset>();
    textureAsset->image = imageId;
    textureAsset->name = subAssetIdentifier.empty()
        ? metadata.path.stem().string()
        : subAssetIdentifier;
    textureAsset->type = Asset::AssetType::Texture;

    return static_cast<const TextureAsset*>(
        assetWarehouseService.StoreAsset(metadata, std::move(textureAsset))
    );
}