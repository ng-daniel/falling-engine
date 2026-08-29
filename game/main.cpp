#include <cmath>

#include "engine/ecs/components/transform.h"
#include "include/generated/asset_ids.h"
#include "engine/core/application.h"

#include "engine/debug/logger.h"
#include "engine/utils/random.h"

int main() {
    Application app = Application("./game/assets");

    /// ASSET TEST
    /// ---------------------------------------------------------------

    AssetManager& assetManager = app.GetAssetManager();

    const ShaderAsset * shader = assetManager.RequestAssetReadOnly<ShaderAsset>(GameAssets::BASICFRAG_SHADER); // Example usage of RequestAsset
    Logger::Info("main", "Finished loading shader asset with ID: " + std::to_string(GameAssets::BASICFRAG_SHADER.GetUUID()));

    const ModelAsset * model = assetManager.RequestAssetReadOnly<ModelAsset>(GameAssets::RYUJIN7_MODEL.GetUUID()); // Example usage of RequestAsset
    Logger::Info("main", "Finished loading model asset with ID: " + std::to_string(model->id));
    Logger::Info("main", "Model has " + std::to_string(model->meshes.size()) + " meshes.");
    const MeshAsset * mesh = assetManager.RequestAssetReadOnly<MeshAsset>(model->meshes[0]);
    Logger::Info("main", "Finished loading mesh asset with ID: " + std::to_string(model->meshes[0]));


    /// ENTITY AND SCENE TEST
    /// ---------------------------------------------------------------

    EcsManager& ecsManager = app.GetECSManager();
    Random::SetSeed(15);
    for (int i = 0; i < 10; i++) {
        const Entity * entity = ecsManager.CreateEntity();
        Logger::Info("main", "Created entity with ID: " + std::to_string(entity->entityId) + "and runtime idx: " + std::to_string(entity->entityRuntimeIdx));
        assert(entity != nullptr);
        ecsManager.AddComponent<Transform>(*entity);
        Logger::Info("main", "Added Transform component to entity with ID: " + std::to_string(entity->entityId));
        Transform * transform = ecsManager.GetComponent<Transform>(*entity);
        Transform::ChangePosition(
            *transform,
            Vector3(
                Random::RandFloat(0.0f, 10.0f),
                Random::RandFloat(0.0f, 10.0f),
                Random::RandFloat(0.0f, 10.0f)
            )
        );
        if (transform) {
            Logger::Info("main", "Transform Data: " + std::to_string(transform->position.x) + ", " + std::to_string(transform->position.y) + ", " + std::to_string(transform->position.z));
        }
    }

    return 0;
}
