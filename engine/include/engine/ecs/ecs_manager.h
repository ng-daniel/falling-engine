#ifndef ENGINE_ECS_ECS_MANAGER_H
#define ENGINE_ECS_ECS_MANAGER_H

#include "engine/ecs/ecs_warehouse.h"

class EcsManager {
public:

    /// ENTITY OPS
    /// ----------------------------------------------

	Entity Create();
	void Destroy(Entity entity);
	bool IsAlive(Entity entity) const;

	Entity * LookupEntity(UUID entityId);
	const Entity * LookupEntity(UUID entityId) const;

    /// COMPONENT OPS
    /// pretty self explanatory these ones
    /// ----------------------------------------------

	template <typename T>
	T * AddComponent(Entity entity) {
		return warehouse.AddComponent<T>(entity);
	}
	template <typename T>
	void RemoveComponent(Entity entity) {
		warehouse.RemoveComponent<T>(entity);
	}
	template <typename T>
	T * GetComponent(Entity entity) {
		return warehouse.GetComponent<T>(entity);
	}
	template <typename T>
	const T * GetComponentReadOnly(Entity entity) const {
		return warehouse.GetComponentReadOnly<T>(entity);
	}
	template <typename T>
	bool HasComponent(Entity entity) const {
		return warehouse.HasComponent<T>(entity);
	}

private:
	EcsWarehouse warehouse;
};

#endif // ENGINE_ECS_ECS_MANAGER_H