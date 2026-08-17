#include "engine/ecs/ecs_structures.h"
#include "engine/ecs/ecs_warehouse.h"
#include "engine/utils/uuid.h"

Entity EcsWarehouse::CreateEntityNew() {
    Entity entity;
    entity.entityId = UUIDGenerator::GenerateUUID();
    return entity;
}

void EcsWarehouse::DeleteEntity() {
    
    
}