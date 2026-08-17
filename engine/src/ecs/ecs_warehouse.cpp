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
        entity.runtimeIdx = freeRuntimeIds.back();
        freeRuntimeIds.pop_back();
    } else {
        entity.runtimeIdx = nextRuntimeId++;
    }

    entity.name = GenerateUniqueEntityName();

    return entity;
}

/**
 * @brief Deletes an entity and all of its components
 * 
 * @param entity 
 */
void EcsWarehouse::DeleteEntity(Entity entity) {
    
    // delete all components associated with this entity
    for (auto& array : componentArrays) {
        if (array) {
            array->DeleteComponent(entity.runtimeIdx);
        }
    }

    // add runtime index back to free list
    freeRuntimeIds.push_back(entity.runtimeIdx);
    // remove entity from entities map
    entities.erase(entity.entityId);
}