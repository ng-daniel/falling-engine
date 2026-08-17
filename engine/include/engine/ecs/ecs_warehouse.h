#ifndef ENGINE_ECS_ECS_WAREHOUSE_H
#define ENGINE_ECS_ECS_WAREHOUSE_H

#include <cassert>
#include <memory>
#include <unordered_map>
#include <vector>

#include "engine/ecs/ecs_structures.h"
#include "engine/ecs/ecs_component_array.h"

/**
 * @brief Manages create, read, and deletion of entities
 * and their associated components in the ECS.
 */
class EcsWarehouse {
public:
    Entity CreateEntityNew();
    void DeleteEntity(Entity entity);
    bool IsAlive(Entity entity) const;
    Entity * FindEntity(UUID entityId);
    const Entity * FindEntityReadOnly(UUID entityId) const;

    template <typename T>
    T * AddComponent(Entity entity) {
        if (!IsAlive(entity)) {
            return nullptr;
        }
        if (!TryAddComponentArray<T>()) {
            // component array already exists for this type
        }
        EcsComponentArray<T> * array = GetArray<T>();
        return array->NewComponent(entity.entityRuntimeIdx);
    }
    
    template <typename T>
    void RemoveComponent(Entity entity) {
        if (!IsAlive(entity)) {
            return;
        }
        EcsComponentArray<T> * array = GetArray<T>();
        if (array) {
            array->DeleteComponent(entity.entityRuntimeIdx);
        }
    }
    void RemoveAllComponents(Entity entity);

    template <typename T>
    T * GetComponent(Entity entity) {
        if (!IsAlive(entity)) {
            return nullptr;
        }
        EcsComponentArray<T> * array = GetArray<T>();
        if (array) {
            return array->GetComponent(entity.entityRuntimeIdx);
        }
        return nullptr;
    }

    template <typename T>
    const T * GetComponentReadOnly(Entity entity) const {
        return GetComponent<T>(entity);
    }

    template <typename T>
    bool HasComponent(Entity entity) const {
        return GetComponentReadOnly<T>(entity) != nullptr;
    }

private:
    // entity runtime ID assignment
    uint32_t nextRuntimeId = 0;
    std::vector<uint32_t> freeRuntimeIds;

    // primary storage maps
    std::unordered_map<UUID, Entity> entities;
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
    const EcsComponentArray<T> * GetArrayReadOnly() const {
        return GetArray<T>();
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