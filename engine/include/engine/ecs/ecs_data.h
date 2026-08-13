#ifndef ENGINE_ECS_ECS_DATA_H
#define ENGINE_ECS_ECS_DATA_H

#include "engine/utils/uuid.h"

/**
 * @brief Just a marker for components
 * 
 */
class Component {};

/**
 * @brief Represents an entity in a scene
 */
class Entity {
    UUID entityId;
};

#endif // ENGINE_ECS_ECS_DATA_H