#pragma once

#include "engine/ecs/components/component.h"
#include "engine/ecs/ecs_structures.h"
#include "engine/ecs/ecs_warehouse.h"
#include "engine/ecs/ecs_component_registry.h"
#include <string>
#include <utility>
#include <vector>

#include "engine/ecs/components/transform.h"
class EcsManager {
public:

    /// ENTITY OPS
    /// ----------------------------------------------

	const Entity * CreateEntity();

	/**
	 * IMPORTANT: only used by the scene serializer/builder, do NOT use anywhere else
	 */
	const Entity * CreateEntity(UUID entityId, std::string name);
	
	/** @brief Queues an entity for destruction at the next FlushRemovals call. */
	void DestroyEntity(Entity entity);
	/**
	 * @brief Applies queued component and entity removals at a safe point.
	 * Queued entities and components remain accessible until this call.
	 * Component references may be invalidated by the removals.
	 */
	void FlushRemovals();
	bool IsEntityAlive(Entity entity) const;

	Entity * GetEntity(UUID entityId);
	Entity * GetEntity(ECS_RID entityRid);
	const Entity * GetEntity(UUID entityId) const;

    /// COMPONENT OPS
    /// pretty self explanatory these ones
    /// ----------------------------------------------

	/**
	 * @brief This is only used by scene serializer/builder
	 * DNI for now idk what im gonna do with scene serialization
	 * 
	 * @param entity 
	 * @param type 
	 * @return Component* 
	 */
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
			componentRegistry.OnCreate(entity, *component);
		}
		return component;
	}
	
	template <typename T>
	T * AddComponent(Entity entity, T initialValue) {
		T* component = warehouse.AddComponent<T>(entity);
		if (component) {
			*component = std::move(initialValue);
			componentRegistry.OnCreate(entity, *component);
		}
		return component;
	}
	
	/** @brief Queues a component for removal at the next FlushRemovals call. */
	template <typename T>
	void RemoveComponent(Entity entity) {
		if (warehouse.GetComponent<T>(entity)) {
			pendingRemovals.push_back({entity.entityId, entity.entityRuntimeIdx, &RemoveComponentNow<T>});
		}
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
	ECSComponentRegistry& GetComponentRegistry() {
		return componentRegistry;
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
	struct PendingRemoval {
		UUID entityId;
		ECS_RID entityRuntimeIdx;
		void (*removeComponent)(EcsManager&, Entity); // nullptr means destroy the entity
	};

	EcsWarehouse warehouse;
	ECSComponentRegistry componentRegistry;
	std::vector<PendingRemoval> pendingRemovals;
	bool flushingRemovals = false;

	template <typename T>
	static void RemoveComponentNow(EcsManager& ecs, Entity entity) {
		if (T* component = ecs.warehouse.GetComponent<T>(entity)) {
			ecs.componentRegistry.OnDestroy(entity, *component);
			ecs.warehouse.RemoveComponent<T>(entity);
		}
	}

	void DestroyEntityNow(Entity entity);
	void DetachEntity(Entity& entity);
};
