#ifndef ENGINE_ECS_ECS_WAREHOUSE_H
#define ENGINE_ECS_ECS_WAREHOUSE_H

#include "engine/utils/uuid.h"
#include "engine/ecs/ecs_structures.h"

#include <unordered_map>

class ECS_Warehouse {
public:
    UUID CreateEntity();
    void DeleteEntity();

private:

};

#endif // ENGINE_ECS_ECS_WAREHOUSE_H