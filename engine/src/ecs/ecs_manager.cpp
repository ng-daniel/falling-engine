#include "engine/ecs/ecs_manager.h"

#include "engine/ecs/components/transform.h"
#include "engine/ecs/components/mesh_renderer.h"
#include "engine/debug/logger.h"

#include <unordered_set>

/**
 * @brief Creates a brand new entity and returns it
 * 
 * @return Entity 
 */
const Entity * EcsManager::CreateEntity() {
	return warehouse.CreateEntityNew();
}
const Entity * EcsManager::CreateEntity(UUID entityId, std::string name) {
	return warehouse.CreateEntityFromScene(entityId, name);
}

/**
 * @brief Deletes an entity and all of its components
 * 
 * @param entity 
 */
void EcsManager::DestroyEntity(Entity entity) {
	// Keep surviving entities from retaining references to a deleted parent or
	// sibling. Children become detached root-level entities.
	Transform* transform = warehouse.GetComponent<Transform>(entity);
	if (transform) {
		DetachEntity(entity);
		while (transform->firstChildEntityId != 0) {
			Entity* child = warehouse.FindEntity(transform->firstChildEntityId);
			if (!child) {
				transform->firstChildEntityId = 0;
				break;
			}
			DetachEntity(*child);
		}
	}
	warehouse.DeleteEntity(entity);
}

/**
 * @brief Checks if an entity is alive/valid by asking the warehouse
 * 
 * @param entity 
 * @return true 
 * @return false 
 */
bool EcsManager::IsEntityAlive(Entity entity) const {
	return warehouse.IsAlive(entity);
}

/**
 * @brief Returns ptr to requested entity via UUID
 * 
 * @param entityId 
 * @return Entity* 
 */
Entity * EcsManager::GetEntity(UUID entityId) {
	return warehouse.FindEntity(entityId);
}
Entity * EcsManager::GetEntity(ECS_RID entityRid) {
	return warehouse.FindEntityByRuntimeId(entityRid);
}
const Entity * EcsManager::GetEntity(UUID entityId) const {
	return warehouse.FindEntityReadOnly(entityId);
}

void EcsManager::RegisterComponents() {
	componentRegistry.RegisterComponent<Transform>(
		"Transform",
		Transform::Serialize,
		Transform::Deserialize
	);
	componentRegistry.RegisterComponent<MeshRenderer>(
		"MeshRenderer",
		MeshRenderer::Serialize,
		MeshRenderer::Deserialize
	);
}

/**
 * @brief Remove an entity from its parent and sibling relationships
 * 
 * @param entity 
 */
void EcsManager::DetachEntity(Entity& entity) {
	Transform* transform = warehouse.GetComponent<Transform>(entity);
	if (!transform || transform->parentEntityId == 0) {
		return;
	}

	Entity* parent = warehouse.FindEntity(transform->parentEntityId);
	Transform* parentTransform = parent ? warehouse.GetComponent<Transform>(*parent) : nullptr;
	Entity* nextSibling = warehouse.FindEntity(transform->nextSiblingEntityId);
	Transform* nextSiblingTransform = nextSibling
		? warehouse.GetComponent<Transform>(*nextSibling)
		: nullptr;
	Entity* prevSibling = warehouse.FindEntity(transform->prevSiblingEntityId);
	Transform* prevSiblingTransform = prevSibling
		? warehouse.GetComponent<Transform>(*prevSibling)
		: nullptr;

	const bool isOnlyChild = transform->nextSiblingEntityId == entity.entityId &&
		transform->prevSiblingEntityId == entity.entityId;
	if (parentTransform && parentTransform->firstChildEntityId == entity.entityId) {
		parentTransform->firstChildEntityId = isOnlyChild ? 0 : transform->nextSiblingEntityId;
	}

	if (!isOnlyChild) {
		if (nextSiblingTransform) {
			nextSiblingTransform->prevSiblingEntityId = transform->prevSiblingEntityId;
		}
		if (prevSiblingTransform) {
			prevSiblingTransform->nextSiblingEntityId = transform->nextSiblingEntityId;
		}
	}

	transform->parentEntityId = 0;
	transform->nextSiblingEntityId = 0;
	transform->prevSiblingEntityId = 0;
}

void EcsManager::Parent(Entity& parent, Entity& child) {
	Transform* parentTransform = warehouse.GetComponent<Transform>(parent);
	Transform* childTransform = warehouse.GetComponent<Transform>(child);
	if (!parentTransform || !childTransform || parent.entityId == child.entityId) {
		return;
	}

	// Walking upward from the prospective parent detects both a descendant
	// relationship and pre-existing corruption that would otherwise loop.
	std::unordered_set<UUID> visited;
	for (Entity* ancestor = &parent; ancestor && visited.insert(ancestor->entityId).second;) {
		if (ancestor->entityId == child.entityId) {
			return;
		}
		Transform* ancestorTransform = warehouse.GetComponent<Transform>(*ancestor);
		ancestor = ancestorTransform && ancestorTransform->parentEntityId != 0
			? warehouse.FindEntity(ancestorTransform->parentEntityId)
			: nullptr;
	}

	DetachEntity(child);
	childTransform->parentEntityId = parent.entityId;

	// resolve sibling relationships and first child reference for the parent

	if (parentTransform->firstChildEntityId == 0) {
		parentTransform->firstChildEntityId = child.entityId;
		childTransform->nextSiblingEntityId = child.entityId;
		childTransform->prevSiblingEntityId = child.entityId;
	} else {

		// insert sibling at tail of circular linked list of siblings
		
		UUID siblingHeadId = parentTransform->firstChildEntityId;
		Entity* siblingHead = warehouse.FindEntity(siblingHeadId);
		Transform* siblingHeadTransform = siblingHead
			? warehouse.GetComponent<Transform>(*siblingHead)
			: nullptr;
		if (!siblingHeadTransform) {
			parentTransform->firstChildEntityId = child.entityId;
			childTransform->nextSiblingEntityId = child.entityId;
			childTransform->prevSiblingEntityId = child.entityId;
			return;
		}
		UUID siblingTailId = siblingHeadTransform->prevSiblingEntityId;
		Entity* siblingTail = warehouse.FindEntity(siblingTailId);
		Transform* siblingTailTransform = siblingTail
			? warehouse.GetComponent<Transform>(*siblingTail)
			: nullptr;
		if (!siblingTailTransform) {
			parentTransform->firstChildEntityId = child.entityId;
			childTransform->nextSiblingEntityId = child.entityId;
			childTransform->prevSiblingEntityId = child.entityId;
			return;
		}
		siblingTailTransform->nextSiblingEntityId = child.entityId;
		childTransform->prevSiblingEntityId = siblingTailId;
		siblingHeadTransform->prevSiblingEntityId = child.entityId;
		childTransform->nextSiblingEntityId = siblingHeadId;
	}
}

void EcsManager::UnParent(Entity& child) {
	DetachEntity(child);
}

/**
 * @brief Computes the world transform of the given entity
 * 
 * @param entity 
 * @return Transform 
 */
Transform EcsManager::ComputeWorldTransform(Entity entity) {
	const Transform * localTransform = warehouse.GetComponent<Transform>(entity);
	if (!localTransform) {
		throw std::runtime_error("Local transform not found for entity");
	}
	Transform worldTransform = *localTransform;
	Entity * currentEntity = &entity;
	while (currentEntity->entityId != warehouse.GetRootEntityId()) {
		// get parent transform
		Entity * parentEntity = warehouse.FindEntity(worldTransform.parentEntityId);
		if (!parentEntity) {
			throw std::runtime_error("Parent entity not found for entity");
			break;
		}
		const Transform * parentTransform = warehouse.GetComponent<Transform>(*parentEntity);
		if (!parentTransform) {
			throw std::runtime_error("Parent transform not found for entity");
			break;
		}
		
		// use parent's family pointers but the aggregated transform matrix
		Transform result = *parentTransform;
		result.matrix = Transform::ComposeTransforms(parentTransform->matrix, worldTransform.matrix);
		worldTransform = result;

		// move up the tree
		currentEntity = parentEntity;
	}
	return worldTransform;
}

Entity* EcsManager::GetParent(Entity& child) {
	Transform* childTransform = warehouse.GetComponent<Transform>(child);
	if (!childTransform) {
		return nullptr;
	}
	UUID parentId = childTransform->parentEntityId;
	if (parentId == 0) {
		return nullptr;
	}
	return warehouse.FindEntity(parentId);
}

std::vector<Entity*> EcsManager::GetChildren(Entity& parent) {
	std::vector<Entity*> children;
	Transform* parentTransform = warehouse.GetComponent<Transform>(parent);
	if (!parentTransform) {
		return children;
	}
	UUID childId = parentTransform->firstChildEntityId;
	if (childId == 0) {
		return children;
	}
	do {
		Entity* child = warehouse.FindEntity(childId);
		if (!child) {
			break;
		}
		children.push_back(child);
		Transform* childTransform = warehouse.GetComponent<Transform>(*child);
		if (!childTransform) {
			break;
		}
		childId = childTransform->nextSiblingEntityId;
	} while (childId != parentTransform->firstChildEntityId);
	return children;
}

void EcsManager::SetScene(Entity& entity) {
	warehouse.SetScene(entity);
}
