#pragma once

#include "engine/ecs/ecs_structures.h"
#include "engine/ecs/ecs_warehouse.h"
#include "engine/ecs/ecs_component_registry.h"
#include <string>

#include "engine/ecs/components/transform.h"
class EcsManager {
public:

    /// ENTITY OPS
    /// ----------------------------------------------

	const Entity * CreateEntity();
	const Entity * CreateEntity(UUID entityId, std::string name);
	void DestroyEntity(Entity entity);
	bool IsEntityAlive(Entity entity) const;

	Entity * GetEntity(UUID entityId);
	Entity * GetEntity(ECS_RID entityRid);
	const Entity * GetEntity(UUID entityId) const;

    /// COMPONENT OPS
    /// pretty self explanatory these ones
    /// ----------------------------------------------

	IComponent * AddComponent(Entity entity, std::string type) {
		if (type == "Transform") {
			return AddComponent<Transform>(entity);
		}
		return nullptr;
	}

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

	const std::unordered_map<UUID, Entity>& GetEntityDump() const {
		return warehouse.GetEntityDump();
	}

	const ComponentInfo * GetComponentInfo(const std::string& type) const {
		return componentRegistry.GetComponentInfo(type);
	}

	void GetAllComponents(Entity entity, std::vector<const IComponent*>& components) const {
		warehouse.GetAllComponents(entity, components);
	}

	template <typename T>
	EntityComponentView<T> GetEntityComponentView() {
		return warehouse.GetEntityComponentView<T>();
	}

	/// COMPONENT REGISTRATION
	/// ----------------------------------------------

	/**
	 * @brief All component type registrations are hardcoded
	 * Maybe I make a better version if I can think of one 
	 */
	void RegisterComponents();

	/// HIERARCHY OPS
	/// ----------------------------------------------

	// Adds child to the end of parent's circular sibling list. Invalid entities,
	// self-parenting, and descendant-parenting requests are ignored.
	void Parent(Entity& parent, Entity& child);
	// Removes child from its current parent. The child becomes root-level.
	void UnParent(Entity& child);
	Transform ComputeWorldTransform(Entity entity);
	Entity* GetParent(Entity& child);
	std::vector<Entity*> GetChildren(Entity& parent);

	/// SCENE OPS
	/// ----------------------------------------------

	void SetScene(Entity& entity);

private:
	EcsWarehouse warehouse;
	ECSComponentRegistry componentRegistry;

	void DetachEntity(Entity& entity);
};
