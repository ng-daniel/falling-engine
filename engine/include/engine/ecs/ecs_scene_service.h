#pragma once

#include "engine/assets/asset_data.h"
#include "engine/assets/asset_manager.h"
#include "engine/ecs/ecs_manager.h"

class ECSSceneService {
public:
	static SceneAsset BuildSceneFromEntities(const EcsManager& ecsManager, UUID rootEntity);
	static void LoadEntitiesFromScene(UUID sceneAssetId, AssetManager& assetManager, EcsManager& ecsManager);
private:
};
