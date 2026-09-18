#pragma once

#include "engine/serialization/jsonarchive.h"
#include <string>

/**
 * @brief Just a marker for components
 *
 */
struct Component {
    static void Serialize(JsonArchive& archive, const Component& component);
    static Component Deserialize(JsonArchive& archive, Component& component);
    virtual std::string GetType() const = 0;
};
