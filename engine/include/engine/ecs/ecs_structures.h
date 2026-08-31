#pragma once

#include "engine/serialization/jsonarchive.h"
#include "engine/utils/uuid.h"
#include <cstdint>

/**
 * @brief Just a marker for components
 * 
 */
struct IComponent {
    static void Serialize(JsonArchive& archive, const IComponent& component);
    static IComponent Deserialize(JsonArchive& archive, IComponent& component);
    virtual std::string GetType() const = 0;
};

/**
 * @brief Represents an entity in a scene
 */
struct Entity {
    UUID entityId;
    uint32_t entityRuntimeIdx; // used for runtime indexing into ECS component arrays
    std::string name;

    void SetName(const std::string& newName) {
        name = newName;
    }
};
