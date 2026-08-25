#pragma once

#include "engine/ecs/ecs_warehouse.h"
#include "engine/ecs/ecs_component_registry.h"

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

	/// COMPONENT REGISTRATION
	/// ----------------------------------------------

	/**
	 * @brief All component type registrations are hardcoded
	 * Maybe I make a better version if I can think of one 
	 */
	void RegisterComponents();

private:
	EcsWarehouse warehouse;
	ECSComponentRegistry componentRegistry;
};
