#include "engine/ecs/ecs_manager.h"

#include "engine/ecs/components/transform.h"

/**
 * @brief Creates a brand new entity and returns it
 * 
 * @return Entity 
 */
Entity EcsManager::Create() {
	return warehouse.CreateEntityNew();
}
Entity EcsManager::Create(UUID entityId, std::string name) {
	return warehouse.CreateEntityFromScene(entityId, name);
}

/**
 * @brief Deletes an entity and all of its components
 * 
 * @param entity 
 */
void EcsManager::Destroy(Entity entity) {
	warehouse.DeleteEntity(entity);
}

/**
 * @brief Checks if an entity is alive/valid by asking the warehouse
 * 
 * @param entity 
 * @return true 
 * @return false 
 */
bool EcsManager::IsAlive(Entity entity) const {
	return warehouse.IsAlive(entity);
}

/**
 * @brief Returns ptr to requested entity via UUID
 * 
 * @param entityId 
 * @return Entity* 
 */
Entity * EcsManager::LookupEntity(UUID entityId) {
	return warehouse.FindEntity(entityId);
}
const Entity * EcsManager::LookupEntity(UUID entityId) const {
	return warehouse.FindEntityReadOnly(entityId);
}

void EcsManager::RegisterComponents() {
	componentRegistry.RegisterComponent<Transform>(
		"Transform",
		Transform::Serialize,
		Transform::Deserialize
	);
}