#ifndef ENGINE_ECS_ECS_WAREHOUSE_H
#define ENGINE_ECS_ECS_WAREHOUSE_H

#include <cassert>
#include <memory>
#include <unordered_map>

#include "engine/ecs/ecs_structures.h"
#include "engine/ecs/ecs_component_array.h"

/**
 * @brief Manages create, read, and deletion of entities
 * and their associated components in the ECS.
 */
class EcsWarehouse {
public:
    Entity CreateEntity();
    void DeleteEntity();

    template <typename T>
    T AddComponent(Entity entity) {
        if (!TryAddComponentArray<T>()) {
            // component array already exists for this type
        }
        EcsComponentArray<T> * array = GetArray<T>();
        return *array->NewComponent(entity.runtimeIdx);
    }
    
    template <typename T>
    void RemoveComponent(Entity entity) {
        EcsComponentArray<T> * array = GetArray<T>();
        if (array) {
            array->DeleteComponent(entity.runtimeIdx);
        }
    }

    template <typename T>
    T GetComponent(Entity entity) {
        EcsComponentArray<T> * array = GetArray<T>();
        if (array) {
            return *array->GetComponent(entity.runtimeIdx);
        }
        return T();
    }

private:
    std::unordered_map<uint32_t, Entity> entities;
    std::vector<std::unique_ptr<IEcsComponentArray>> componentArrays;
    
    template <typename T>
    uint32_t GetComponentTypeId() {
        static uint32_t typeId = componentArrays.size();
        return typeId;
    }

    template <typename T>
    EcsComponentArray<T> * GetArray() {
        uint32_t typeId = GetComponentTypeId<T>();
        if (typeId >= componentArrays.size()) {
            return nullptr;
        }
        return static_cast<EcsComponentArray<T> *>(componentArrays[typeId].get());
    }

    template <typename T>
    bool TryAddComponentArray() {
        uint32_t typeId = GetComponentTypeId<T>();
        if (typeId >= componentArrays.size()) {
            componentArrays.push_back(std::make_unique<EcsComponentArray<T>>());
            assert(componentArrays.size() == typeId + 1);
            return true;
        }
        else {
            // component array already exists for this type
            return false;
        }
    }
};

#endif // ENGINE_ECS_ECS_WAREHOUSE_H