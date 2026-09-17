#pragma once

#include "engine/assets/asset_manager.h"
#include "engine/ecs/ecs_structures.h"
#include "engine/ecs/ecs_manager.h"

/**
 * @brief Miscelaneous utilities for the ECS and scene system
 * 
 */
class ECSExtras {
public:
    static Entity * BuildEntityFromModel(AssetManager& assetManager, EcsManager& ecsManager, UUID modelId);
};