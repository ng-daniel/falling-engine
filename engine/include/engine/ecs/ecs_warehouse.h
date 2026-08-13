#ifndef ENGINE_ECS_ECS_WAREHOUSE_H
#define ENGINE_ECS_ECS_WAREHOUSE_H

#include "engine/utils/uuid.h"
#include "engine/ecs/ecs_data.h"

#include <unordered_map>

class ECS_Warehouse {
public:
    UUID CreateEntity();
    void DeleteEntity();

private:
    std::unordered_map<UUID, long> sparseMap;
    std::vector<Component> denseMap;
};

#endif // ENGINE_ECS_ECS_WAREHOUSE_H