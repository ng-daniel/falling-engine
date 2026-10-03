#pragma once

#include "engine/ecs/components/component.h"
#include "engine/ecs/ecs_structures.h"
#include "engine/ecs/ecs_warehouse.h"
#include "engine/ecs/ecs_component_registry.h"
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

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

	Component * AddComponent(Entity entity, std::string type) {
		if (type == "Transform") {
			return AddComponent<Transform>(entity);
		}
		return nullptr;
	}

	template <typename T>
	T * AddComponent(Entity entity) {
		T* component = warehouse.AddComponent<T>(entity);
		if (component) {
			if (ComponentLifecycle* lifecycle = FindComponentLifecycle<T>()) {
				lifecycle->onCreate(entity);
			}
		}
		return component;
	}
	template <typename T>
	T * AddComponent(Entity entity, T initialValue) {
		T* component = warehouse.AddComponent<T>(entity);
		if (component) {
			*component = std::move(initialValue);
			if (ComponentLifecycle* lifecycle = FindComponentLifecycle<T>()) {
				lifecycle->onCreate(entity);
			}
		}
		return component;
	}
	template <typename T>
	void RemoveComponent(Entity entity) {
		if (warehouse.HasComponent<T>(entity)) {
			if (ComponentLifecycle* lifecycle = FindComponentLifecycle<T>()) {
				lifecycle->onDestroy(entity);
			}
		}
		warehouse.RemoveComponent<T>(entity);
	}

	// Lifecycle hooks are registered per component type, never per instance.
	template <typename T>
	void RegisterComponentLifecycle(
		std::function<void(Entity)> onCreate,
		std::function<void(Entity)> onDestroy
	) {
		if (FindComponentLifecycle<T>()) {
			throw std::runtime_error("Component lifecycle already registered");
		}
		componentLifecycles.push_back({std::type_index(typeid(T)), std::move(onCreate), std::move(onDestroy)});
	}

	template <typename T>
	void UnregisterComponentLifecycle() {
		const std::type_index type(typeid(T));
		std::erase_if(componentLifecycles, [type](const ComponentLifecycle& lifecycle) {
			return lifecycle.type == type;
		});
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

	void GetAllComponents(Entity entity, std::vector<const Component*>& components) const {
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
	struct ComponentLifecycle {
		std::type_index type;
		std::function<void(Entity)> onCreate;
		std::function<void(Entity)> onDestroy;
	};

	template <typename T>
	ComponentLifecycle* FindComponentLifecycle() {
		const std::type_index type(typeid(T));
		auto it = std::find_if(componentLifecycles.begin(), componentLifecycles.end(),
			[type](const ComponentLifecycle& lifecycle) { return lifecycle.type == type; });
		return it == componentLifecycles.end() ? nullptr : &*it;
	}

	EcsWarehouse warehouse;
	ECSComponentRegistry componentRegistry;
	std::vector<ComponentLifecycle> componentLifecycles;

	void DetachEntity(Entity& entity);
};
