#pragma once

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
    uint32_t entityRuntimeIdx; // used for runtime indexing into ECS component arrays
    std::string name;
};
