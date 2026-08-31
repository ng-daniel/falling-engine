#include "engine/ecs/ecs_extras.h"

Entity ECSExtras::BuildEntityFromModel(AssetManager& assetManager, EcsManager& ecsManager, UUID modelId) {
    ModelAsset * model = assetManager.RequestAsset<ModelAsset>(modelId);
    Entity * root = ecsManager.LookupEntity(ecsManager.CreateEntity()->entityId);
    root->SetName(model->name);

    

    return *root;
}