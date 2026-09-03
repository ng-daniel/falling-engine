#include "engine/ecs/ecs_structures.h"
#include "engine/ecs/ecs_warehouse.h"
#include <string>
#include "engine/utils/uuid.h"
#include "engine/ecs/components/transform.h"

namespace {
    std::string GenerateUniqueEntityName() {
        static int entityCounter = 0;
        return "Entity_" + std::to_string(entityCounter++);
    }
}

/**
 * @brief Initializes brand new entity
 * 
 * @return Entity 
 */
const Entity * EcsWarehouse::CreateEntityNew() {
    return CreateEntity(
        UUIDGenerator::GenerateUUID(),
        GenerateUniqueEntityName()
    );
}
const Entity * EcsWarehouse::CreateEntityFromScene(UUID entityId, std::string name) {
    return CreateEntity(entityId, name);
}

Entity * EcsWarehouse::CreateEntity(UUID uuid, std::string name) {
    Entity entity;
    entity.entityId = uuid;
    entity.name = name;

    // assign runtime index
    if (!freeRuntimeIds.empty()) {
        entity.entityRuntimeIdx = freeRuntimeIds.back();
        freeRuntimeIds.pop_back();
    } else {
        entity.entityRuntimeIdx = nextRuntimeId++;
    }

    uuidToEntityMap[entity.entityId] = entity;
    runtimeIdToEntityMap[entity.entityRuntimeIdx] = entity;
    
    // add transform by default
    AddComponent<Transform>(entity);

    return &uuidToEntityMap[entity.entityId];
}

/**
 * @brief Deletes an entity and all of its components
 * 
 * @param entity 
 */
void EcsWarehouse::DeleteEntity(Entity entity) {
    if (!IsAlive(entity)) {
        return;
    }
    
    // delete all components associated with this entity
    RemoveAllComponents(entity);

    // add runtime index back to free list
    freeRuntimeIds.push_back(entity.entityRuntimeIdx);
    // remove entity from entities map
    uuidToEntityMap.erase(entity.entityId);
    runtimeIdToEntityMap.erase(entity.entityRuntimeIdx);
}

/**
 * @brief Determines if an entity is alive by 
 * 1. checking if it exists in the entities map and
 * 2. if its runtime index matches
 * mostly for like a sanity/safety check kinda thing
 * 
 * @param entity 
 * @return true 
 * @return false 
 */
bool EcsWarehouse::IsAlive(Entity entity) const {
    auto found = uuidToEntityMap.find(entity.entityId);
    return found != uuidToEntityMap.end();
}

/**
 * @brief Returns entity given UUID
 * 
 * @param entityId 
 * @return Entity* 
 */
Entity * EcsWarehouse::FindEntity(UUID entityId) {
    auto found = uuidToEntityMap.find(entityId);
    if (found == uuidToEntityMap.end()) {
        return nullptr;
    }
    return &found->second;
}
const Entity * EcsWarehouse::FindEntityReadOnly(UUID entityId) const {
    auto found = uuidToEntityMap.find(entityId);
    if (found == uuidToEntityMap.end()) {
        return nullptr;
    }
    return &found->second;
}

Entity * EcsWarehouse::FindEntityByRuntimeId(ECS_RID runtimeId) {
    auto found = runtimeIdToEntityMap.find(runtimeId);
    if (found == runtimeIdToEntityMap.end()) {
        return nullptr;
    }
    return &found->second;
}

/**
 * @brief Loop through all component arrays and delete the
 * component associated with the given entity if int exists
 * 
 * @param entity 
 */
void EcsWarehouse::RemoveAllComponents(Entity entity) {
    for (auto& array : componentArrays) {
        if (array) {
            array->DeleteComponent(entity.entityRuntimeIdx);
        }
    }
}

/**
 * @brief Gets all components
 * 
 * @param entity 
 * @param components 
 *
 * @details Iterate over each component array, if entity has component there, add it to the components vector
 * component vector is readonly, getallcomponents should not be used for modifying stuff
 */
void EcsWarehouse::GetAllComponents(Entity entity, std::vector<const IComponent*>& components) const {
    int numComponentTypes = componentArrays.size();
    for (int i = 0; i < numComponentTypes; i++) {
        auto& array = componentArrays[i];
        if (array) {
            const IComponent* component = array->GetComponent(entity.entityRuntimeIdx);
            if (component) {
                components.push_back(component);
            }
        }
    }
}
