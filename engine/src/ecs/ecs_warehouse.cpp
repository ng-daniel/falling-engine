#include "engine/ecs/ecs_structures.h"
#include "engine/ecs/ecs_warehouse.h"
#include "engine/utils/uuid.h"

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
Entity EcsWarehouse::CreateEntityNew() {
    Entity entity;
    
    // generate UUID
    entity.entityId = UUIDGenerator::GenerateUUID();
    
    // assign runtime index
    if (!freeRuntimeIds.empty()) {
        entity.entityRuntimeIdx = freeRuntimeIds.back();
        freeRuntimeIds.pop_back();
    } else {
        entity.entityRuntimeIdx = nextRuntimeId++;
    }
    entity.name = GenerateUniqueEntityName();

    entities[entity.entityId] = entity;
    return entity;
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
    entities.erase(entity.entityId);
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
    auto found = entities.find(entity.entityId);
    return (
        found != entities.end() &&
        found->second.entityRuntimeIdx == entity.entityRuntimeIdx
    );
}

/**
 * @brief Returns entity given UUID
 * 
 * @param entityId 
 * @return Entity* 
 */
Entity * EcsWarehouse::FindEntity(UUID entityId) {
    auto found = entities.find(entityId);
    if (found == entities.end()) {
        return nullptr;
    }
    return &found->second;
}
const Entity * EcsWarehouse::FindEntityReadOnly(UUID entityId) const {
    auto found = entities.find(entityId);
    if (found == entities.end()) {
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