#pragma once

#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include "engine/ecs/components/component.h"

/**
 * @brief Util class with general component registry functionality
 */
struct ComponentInfo {
    std::string type;
    std::function<void(JsonArchive&, const Component&)> serializeFunc;
    std::function<void(JsonArchive&, Component&)> deserializeFunc;
};

class ECSComponentRegistry {
public:
    template <typename T>
    void RegisterComponent(
        std::string type,
        void (*serializeFunc)(JsonArchive&, const T&),
        void (*deserializeFunc)(JsonArchive&, T&)
    ) {
        static_assert(std::is_base_of_v<Component, T>, "Registered component type must derive from IComponent");
        if (componentRegistry.find(type) != componentRegistry.end()) {
            throw std::runtime_error("Component type already registered: " + type);
        }
        // create two lambda functions
        componentRegistry[type] = {
            type,
            [serializeFunc](JsonArchive& archive, const Component& component) {
                serializeFunc(archive, static_cast<const T&>(component));
            },
            [deserializeFunc](JsonArchive& archive, Component& component) {
                deserializeFunc(archive, static_cast<T&>(component));
            }
        };
    }
    const ComponentInfo * GetComponentInfo(const std::string& type) const {
        auto it = componentRegistry.find(type);
        if (it != componentRegistry.end()) {
            return &(it->second);
        }
        return nullptr;
    }
private:
    std::unordered_map<std::string, ComponentInfo> componentRegistry;
};
