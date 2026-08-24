#pragma once

#include "engine/assets/asset_data.h"
#include "engine/ecs/ecs_manager.h"

class ECSSceneService {
public:
	static SceneAsset BuildSceneFromEntities(const EcsManager& ecsManager, UUID rootEntity);
	static void LoadEntitiesFromScene(const SceneAsset& scene, EcsManager& ecsManager);
private:
};
