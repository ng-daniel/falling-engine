#ifndef ENGINE_ECS_ECS_STRUCTURES_H
#define ENGINE_ECS_ECS_STRUCTURES_H

#include "engine/utils/uuid.h"
#include <cstdint>

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
    uint32_t runtimeIdx; // used for runtime indexing into ECS component arrays
};

#endif // ENGINE_ECS_ECS_STRUCTURES_H