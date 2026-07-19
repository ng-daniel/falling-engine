#include "engine/assets/asset_importer_service.h"

#include "engine/assets/importers/image_importer.h"
#include "engine/assets/importers/shader_importer.h"
#include "engine/assets/importers/model_importer.h"

#include "engine/assets/asset_helpers.h"


std::vector<std::unique_ptr<Asset>> AssetImporterService::ImportSourceAsset(SourceAssetMetadata& metadata) {
    try {
        std::vector<std::unique_ptr<Asset>> assets;
        switch (GetAssetTypeFromString(metadata.type)) {
            case Asset::AssetType::Image:
                assets = ImageImporter::LoadAsset(metadata);
                break;
            case Asset::AssetType::Shader:
                assets = ShaderImporter::LoadAsset(metadata);
                break;
            case Asset::AssetType::Model:
                assets = ModelImporter::LoadAsset(metadata);
                break;
            default:
                throw std::runtime_error("Unsupported asset type for import: " + metadata.type);
        }
        return assets;

    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Failed to import asset: " + metadata.path.string() + ". " + e.what());
    }
}