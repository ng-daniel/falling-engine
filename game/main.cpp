#include <cmath>

#include "include/generated/asset_ids.h"
#include "engine/core/application.h"

#include "engine/debug/logger.h"

int main() {
    Application app = Application("./game/assets");
    AssetManager& assetManager = app.GetAssetManager();

    // TESTING
    const ShaderAsset * shader = assetManager.RequestAssetReadOnly<ShaderAsset>(GameAssets::BASICFRAG_SHADER); // Example usage of RequestAsset
    Logger::Info("main", "Finished loading shader asset with ID: " + std::to_string(GameAssets::BASICFRAG_SHADER.GetUUID()));

    const ModelAsset * model = assetManager.RequestAssetReadOnly<ModelAsset>(GameAssets::RYUJIN7_MODEL.GetUUID()); // Example usage of RequestAsset
    Logger::Info("main", "Finished loading model asset with ID: " + std::to_string(model->id));
    Logger::Info("main", "Model has " + std::to_string(model->meshes.size()) + " meshes.");
    const MeshAsset * mesh = assetManager.RequestAssetReadOnly<MeshAsset>(model->meshes[0]);
    Logger::Info("main", "Finished loading mesh asset with ID: " + std::to_string(model->meshes[0]));

    return 0;
}
