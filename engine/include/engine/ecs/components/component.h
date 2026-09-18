#pragma once

#include "engine/serialization/jsonarchive.h"
#include <string>

/**
 * @brief Just a marker for components
 *
 */
struct Component {
    virtual std::string GetType() const = 0;
};

class ComponentSerializer {
public:
    static void Serialize(JsonArchive& archive, const Component& component);
    static void Deserialize(JsonArchive& archive, Component& component);
};
