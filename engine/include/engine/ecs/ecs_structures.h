#ifndef ENGINE_ECS_ECS_STRUCTURES_H
#define ENGINE_ECS_ECS_STRUCTURES_H

#include "engine/utils/uuid.h"
#include <cstdint>

/**
 * @brief Just a marker for components
 * 
 */
struct IComponent {
    UUID componentId;
    std::string name;
};

/**
 * @brief Represents an entity in a scene
 */
struct Entity {
    UUID entityId;
    uint32_t runtimeIdx; // used for runtime indexing into ECS component arrays
    std::string name;
};

#endif // ENGINE_ECS_ECS_STRUCTURES_H