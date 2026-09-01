#include "engine/ecs/ecs_extras.h"

#include "engine/ecs/components/mesh_renderer.h"
#include "engine/ecs/components/transform.h"

namespace {
    /**
     * @brief Recursively builds an entity hierarchy from the given model node.
     * 
     * @param modelNode 
     * @param parentId 
     * @param ecsManager 
     */
    void BuildModelNode(const ModelNode& modelNode, UUID parentId, EcsManager& ecsManager) {    
        
        // create entity for node
        Entity* entity = ecsManager.LookupEntity(ecsManager.CreateEntity()->entityId);
        entity->SetName(modelNode.name);

        // apply transform data
        Transform* transform = ecsManager.GetComponent<Transform>(*entity);
        transform->position = modelNode.relativePosition;
        transform->rotation = modelNode.relativeRotation;
        transform->scale = modelNode.relativeScale;

        // add mesh renderer if the node has a mesh
        if (modelNode.hasMesh) {
            MeshRenderer* meshRenderer = ecsManager.AddComponent<MeshRenderer>(*entity);
            meshRenderer->SetMesh(modelNode.meshId);
        }

        // parent the entity to its parent in the hierarchy
        Entity* parent = ecsManager.LookupEntity(parentId);
        ecsManager.Parent(*parent, *entity);

        // its recursing time
        for (const ModelNode& child : modelNode.children) {
            BuildModelNode(child, entity->entityId, ecsManager);
        }
    }
}

/**
 * @brief From the given model asset, build an entity hierarchy in the ECS.
 * This function will create a root entity for the model and recursively
 * create child entities for each node in the model's hierarchy.
 * Also applies the relative transform data from model asset to entity transform.
 *
 * @param assetManager
 * @param ecsManager
 * @param modelId
 * @return Entity*
 */
Entity * ECSExtras::BuildEntityFromModel(AssetManager& assetManager, EcsManager& ecsManager, UUID modelId) {
    // fetch model
    ModelAsset * model = assetManager.RequestAsset<ModelAsset>(modelId);
    
    // create model root
    const UUID rootId = ecsManager.CreateEntity()->entityId;
    Entity * root = ecsManager.LookupEntity(rootId);
    root->SetName(model->name);

    // build hierarchy
    for (const ModelNode& tree : model->trees) {
        BuildModelNode(tree, rootId, ecsManager);
    }

    return ecsManager.LookupEntity(rootId);
}
