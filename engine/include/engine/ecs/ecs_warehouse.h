#pragma once

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
    const Entity * CreateEntityNew();
    const Entity * CreateEntityFromScene(UUID entityId, std::string name);
    void DeleteEntity(Entity entity);
    bool IsAlive(Entity entity) const;
    Entity * FindEntity(UUID entityId);
    const Entity * FindEntityReadOnly(UUID entityId) const;

    Entity * FindEntityByRuntimeId(ECS_RID runtimeId);

    template <typename T>
    T * AddComponent(Entity entity) {
        if (!IsAlive(entity)) {
            return nullptr;
        }
        TryAddComponentArray<T>();
        EcsComponentArray<T> * array = GetArray<T>();
        return array->CreateComponent(entity.entityRuntimeIdx);
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
        if (!IsAlive(entity)) {
            return nullptr;
        }
        const EcsComponentArray<T> * array = GetArrayReadOnly<T>();
        if (array) {
            return array->GetComponentReadOnly(entity.entityRuntimeIdx);
        }
        return nullptr;
    }
    void GetAllComponents(Entity entity, std::vector<const IComponent*>& components) const;

    template <typename T>
    bool HasComponent(Entity entity) const {
        return GetComponentReadOnly<T>(entity) != nullptr;
    }

    /**
     * @brief Access a read only view of all entities
     * 
     * @return const std::unordered_map<UUID, Entity>& 
     */
    const std::unordered_map<UUID, Entity>& GetEntityDump() const {
        return uuidToEntityMap;
    }

    /**
     * @brief Pull the raw dense entity array, without knowledge of who owns each component.
     * Only use for component-wide operations (like physics updates)
     * 
     * @tparam T 
     * @return std::vector<T>& 
     */
    template <typename T>
    std::vector<T>& GetAllComponentsOfType() {
        std::vector<T>& denseArray = GetArray<T>()->GetDenseArray();
        return denseArray;
    }

    UUID GetRootEntityId() const {
        return rootEntityId;
    }

    void SetScene(Entity& entity) {
        rootEntityId = entity.entityId;
    }

    template <typename T>
    EntityComponentView<T> GetEntityComponentView() {
        EcsComponentArray<T>* array = GetArray<T>();
        if (!array) {
            return {};
        }
        return array->GetEntityComponentView();
    }

private:
    // entity runtime ID assignment
    ECS_RID nextRuntimeId = 0;
    std::vector<ECS_RID> freeRuntimeIds;

    // primary storage maps
    UUID rootEntityId;
    std::unordered_map<UUID, Entity> uuidToEntityMap;
    std::unordered_map<ECS_RID, Entity> runtimeIdToEntityMap;
    std::vector<std::unique_ptr<IEcsComponentArray>> componentArrays;
    
    Entity * CreateEntity(UUID uuid, std::string name);

    template <typename T>
    const uint32_t GetComponentTypeId() const {
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
        uint32_t typeId = GetComponentTypeId<T>();
        if (typeId >= componentArrays.size()) {
            return nullptr;
        }
        return static_cast<const EcsComponentArray<T> *>(componentArrays[typeId].get());
    }

    template <typename T>
    void TryAddComponentArray() {
        uint32_t typeId = GetComponentTypeId<T>();
        if (typeId >= componentArrays.size()) {
            // add new component array
            componentArrays.push_back(std::make_unique<EcsComponentArray<T>>());
            assert(componentArrays.size() == typeId + 1);
        }
        // component array already exists
    }
};
