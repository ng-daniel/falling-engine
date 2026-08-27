#include "engine/ecs/ecs_scene_service.h"
#include "engine/assets/asset_data.h"
#include "engine/assets/asset_manager.h"
#include <unordered_map>
#include <vector>

namespace {
    void LoadEntityToRuntime(const EntityData& entity, EcsManager& ecsManager) {
        const Entity * entityRuntime = ecsManager.CreateEntity(entity.entityId, entity.name);
        for (const auto& componentData : entity.components) {
            const ComponentInfo * componentInfo = ecsManager.GetComponentInfo(componentData.type);
            if (!componentInfo) {
                throw std::runtime_error("Component type not registered: " + componentData.type);
            }
            JsonArchive tempArchive(JsonArchive::Mode::Reading);
            tempArchive.OpenFromMemory(componentData.data.dump());
            IComponent * component = ecsManager.AddComponent(*entityRuntime, componentData.type);
            componentInfo->deserializeFunc(tempArchive, *component);
        }
    }

    void BuildEntityDataFromRuntime(const Entity& entity, EntityData& entityData, const EcsManager& ecsManager) {
        entityData.entityId = entity.entityId;
        entityData.name = entity.name;

        std::vector<const IComponent*> components;
        ecsManager.GetAllComponents(entity, components);
        entityData.components.reserve(components.size());
        
        // loop through all components, serialize them, and dump them into array
        int i = 0;
        for (const IComponent* component : components) {
            const ComponentInfo* componentInfo = ecsManager.GetComponentInfo(component->GetType());
            if (!componentInfo) {
                throw std::runtime_error("Component type not registered: " + component->GetType());
            }
            JsonArchive archive(JsonArchive::Mode::Writing);
            componentInfo->serializeFunc(archive, *component);
            entityData.components.push_back({
                component->GetType(),
                archive.Dump()
            });
            i++;
        }
    }
}

SceneAsset ECSSceneService::BuildSceneFromEntities(const EcsManager& ecsManager, UUID rootEntity) {
    SceneAsset sceneAsset;

    // readonly view into entity array (like a snapshot)
    const std::unordered_map<UUID, Entity>& entityDump = ecsManager.GetEntityDump();
    sceneAsset.entities.reserve(entityDump.size());

    bool foundRoot = false;
    for (const auto& [uuid, entity] : entityDump) {
        if (foundRoot || entity.entityId == rootEntity) {
            sceneAsset.rootEntity = entity.entityId;
            foundRoot = true;
        }
        // simultaneously build entity data and add it to the end of the entity list
        BuildEntityDataFromRuntime(
            entity, 
            sceneAsset.entities.emplace_back(),
            ecsManager
        );
    }
    return sceneAsset;
}

void ECSSceneService::LoadEntitiesFromScene(UUID sceneAssetId, AssetManager& assetManager, EcsManager& ecsManager) {
    const SceneAsset* sceneAsset = assetManager.RequestAssetReadOnly<SceneAsset>(sceneAssetId);
    if (!sceneAsset) {
        throw std::runtime_error("Failed to load scene asset with ID: " + std::to_string(sceneAssetId));
    }
    for (const auto& entityData : sceneAsset->entities) {
        LoadEntityToRuntime(entityData, ecsManager);
    }
}
