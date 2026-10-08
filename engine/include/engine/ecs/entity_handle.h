#pragma once

#include "engine/ecs/ecs_manager.h"

/**
 * IMPORTANT READ BOTTOM THIS CLASS IS OBSOLETE!!!
 * 
 * @brief An entity reference that is basically a wrapper for ECS commandss
 * I thought it was pointers but it was actually get operations in a trenchcoat!
 * Honestly thats what pointers basically are tbh
 * @details nearly every operation here re-fetches both the entity and its 
 * components from the ECS manager.
 * luckyly the ECS system is built so that entity lookups and component accesses are constant time
 *
 * OBSOLETE FOR NOW - SEE SCRIPT MANAGER'S SCRIPT COMPONENT
 * since this is obsolete we can say that components can't delete or add new separate components
 * during runtime
 */
class EntityHandle {
public:
    EntityHandle() = default;
    EntityHandle(EcsManager& ecs, UUID entityId) : ecs(&ecs), entityId(entityId) {}

    UUID GetUUID() const { return entityId; }
    bool IsValid() const { return Resolve() != nullptr; }
    explicit operator bool() const { return IsValid(); }

    template <typename T>
    T* GetComponent() const {
        Entity* entity = Resolve();
        return entity ? ecs->GetComponent<T>(*entity) : nullptr;
    }

    template <typename T>
    const T* GetComponentReadOnly() const {
        Entity* entity = Resolve();
        return entity ? ecs->GetComponentReadOnly<T>(*entity) : nullptr;
    }

    template <typename T>
    bool HasComponent() const {
        Entity* entity = Resolve();
        return entity && ecs->HasComponent<T>(*entity);
    }

    template <typename T>
    T* AddComponent() const {
        Entity* entity = Resolve();
        return entity ? ecs->AddComponent<T>(*entity) : nullptr;
    }

    template <typename T>
    void RemoveComponent() const {
        if (Entity* entity = Resolve()) {
            ecs->RemoveComponent<T>(*entity);
        }
    }

    void Destroy() const {
        if (Entity* entity = Resolve()) {
            ecs->DestroyEntity(*entity);
        }
    }

    bool operator==(const EntityHandle&) const = default;

private:
    EcsManager* ecs = nullptr;
    UUID entityId = INVALID_UUID;

    Entity* Resolve() const {
        return ecs && entityId != INVALID_UUID ? ecs->GetEntity(entityId) : nullptr;
    }
};
