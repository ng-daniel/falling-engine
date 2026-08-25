#include "engine/ecs/ecs_scene_service.h"
#include "engine/assets/asset_data.h"
#include "engine/assets/asset_manager.h"

SceneAsset ECSSceneService::BuildSceneFromEntities(const EcsManager& ecsManager, UUID rootEntity) {
    
}

void ECSSceneService::LoadEntitiesFromScene(UUID sceneAssetId, AssetManager& assetManager, EcsManager& ecsManager) {
    const SceneAsset* sceneAsset = assetManager.RequestAssetReadOnly<SceneAsset>(sceneAssetId);
    if (!sceneAsset) {
        throw std::runtime_error("Failed to load scene asset with ID: " + std::to_string(sceneAssetId));
    }
    for (const auto& entityData : sceneAsset->entities) {
        Entity entity = ecsManager.Create();
        for (const auto& componentData : entityData.components) {
        }
    }

}