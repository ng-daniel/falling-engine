#pragma once

#include "engine/assets/asset_data.h"
#include "engine/ecs/ecs_manager.h"

class ECSSceneService {
public:
	static SceneAsset Serialize(const EcsManager& ecsManager, UUID rootEntity);
	static void Deserialize(const SceneAsset& scene, EcsManager& ecsManager);
};
