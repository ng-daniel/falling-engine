#include <cmath>

#include "engine/ecs/components/transform.h"
#include "include/generated/asset_ids.h"
#include "engine/core/application.h"

#include "engine/ecs/ecs_extras.h"

#include "engine/debug/logger.h"
#include "engine/utils/random.h"

#include "engine/utils/quaternion.h"

void PrintModelNodeTree(ModelNode node, int depth = 0) {
    std::string indent(depth * 2, ' ');
    Logger::Info("main", indent + "Node name: " + node.name + ", Mesh ID: " + std::to_string(node.meshId));
    Logger::Info("main", "Position: (" +
        std::to_string(node.relativePosition.x) + ", " +
        std::to_string(node.relativePosition.y) + ", " +
        std::to_string(node.relativePosition.z) + ")");
    Logger::Info("main", indent + "Rotation: (" +
        std::to_string(node.relativeRotation.x) + ", " +
        std::to_string(node.relativeRotation.y) + ", " +
        std::to_string(node.relativeRotation.z) + ")");
    Logger::Info("main", indent + "Scale: (" +
        std::to_string(node.relativeScale.x) + ", " +
        std::to_string(node.relativeScale.y) + ", " +
        std::to_string(node.relativeScale.z) + ")");
    for (auto& child : node.children) {
        PrintModelNodeTree(child, depth + 1);
    }
}

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
    Logger::Info("main", "Model first mesh ID: " + std::to_string(model->meshes[0]));
    Logger::Info("main", "Finished loading mesh asset with ID: " + std::to_string(model->meshes[0]));
    const MeshAsset * mesh = assetManager.RequestAssetReadOnly<MeshAsset>(model->meshes[0]);
    Logger::Info("main", "Mesh has " + std::to_string(mesh->primitives.size()) + " primitives.");
    Logger::Info("main", "Mesh primitive has " + std::to_string(mesh->primitives[0].vertices.size()) + " vertices.");
    Logger::Info("main", "Model tree mesh root: " + std::to_string(model->trees[0].meshId));
    for (auto& rootNode : model->trees) {
        PrintModelNodeTree(rootNode);
    }

    /// ENTITY AND SCENE TEST
    /// ---------------------------------------------------------------

    EcsManager& ecsManager = app.GetECSManager();
    Entity * rootEntity = ecsManager.LookupEntity(ecsManager.CreateEntity()->entityId);
    ecsManager.SetScene(*rootEntity);
    Logger::Info("main", "Created root entity with ID: " + std::to_string(rootEntity->entityId) + "and runtime idx: " + std::to_string(rootEntity->entityRuntimeIdx));

    Random::SetSeed(15);
    for (int i = 0; i < 50; i++) {
        Entity * entity = ecsManager.LookupEntity(ecsManager.CreateEntity()->entityId);
        ecsManager.Parent(*rootEntity, *entity); // Parent the newly created entity to the root entity
        Logger::Info("main", "Created entity with ID: " + std::to_string(entity->entityId) + "and runtime idx: " + std::to_string(entity->entityRuntimeIdx));
        assert(entity != nullptr);
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
            const Vector3 position = transform->GetPosition();
            Logger::Info("main", "Transform Data: " + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z));
        }
    }
    // Transform * rootTransform = ecsManager.GetComponent<Transform>(*rootEntity);
    // while (rootTransform->firstChildEntityId != 0) {
    //     Entity * entity = ecsManager.LookupEntity(rootTransform->firstChildEntityId);
    //     if (entity) {
    //         Logger::Info("main", "Destroying child entity ID: " + std::to_string(entity->entityId) + " and runtime idx: " + std::to_string(entity->entityRuntimeIdx));
    //     }
    //     ecsManager.DestroyEntity(*entity);
    //     // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // }
    
    /// MODEL BUILDER TEST
    /// ---------------------------------------------------------------
    
    Entity * modelRootEntity = ECSExtras::BuildEntityFromModel(app.GetAssetManager(), ecsManager, GameAssets::RYUJIN7_MODEL.GetUUID());
    if (modelRootEntity) {
        Logger::Info("main", "Created model root entity with ID: " + std::to_string(modelRootEntity->entityId) + " and runtime idx: " + std::to_string(modelRootEntity->entityRuntimeIdx));
    }
    ecsManager.Parent(*rootEntity, *modelRootEntity);
    Transform  * modelRootTransform = ecsManager.GetComponent<Transform>(*modelRootEntity);
    // Transform::ChangePosition(
    //     *modelRootTransform,
    //     Vector3(0.0, 0.0, 1.0)
    // );
    // // 90 degree rotation along the Y-axis via quaternions
    // Transform::SetRotation(
    //     *modelRootTransform,
    //     Quaternion::EulerToQuaternion(90.0, 0.0, 0.0)
    // );
    
    std::vector<Entity*> children = ecsManager.GetChildren(*modelRootEntity);
    Logger::Info("main", "Number of children: " + std::to_string(children.size()));
    for (const Entity* child : children) {
        Logger::Info("main", "\nChild Entity Name: " + child->name);

        Transform childTransform = *ecsManager.GetComponent<Transform>(*child);
        Logger::Info("main", "Child Local Transform: " + std::to_string(childTransform.GetPosition().x) + ", " + std::to_string(childTransform.GetPosition().y) + ", " + std::to_string(childTransform.GetPosition().z));
        Logger::Info("main", "Child Local Rotation: " + std::to_string(childTransform.GetRotation().x) + ", " + std::to_string(childTransform.GetRotation().y) + ", " + std::to_string(childTransform.GetRotation().z) + ", " + std::to_string(childTransform.GetRotation().w));

        Transform worldTransform = ecsManager.ComputeWorldTransform(*child);
        Logger::Info("main", "World Transform: " + std::to_string(worldTransform.GetPosition().x) + ", " + std::to_string(worldTransform.GetPosition().y) + ", " + std::to_string(worldTransform.GetPosition().z));
        Logger::Info("main", "World Rotation: " + std::to_string(worldTransform.GetRotation().x) + ", " + std::to_string(worldTransform.GetRotation().y) + ", " + std::to_string(worldTransform.GetRotation().z) + ", " + std::to_string(worldTransform.GetRotation().w));
    }
    app.Run();

    return 0;
}
