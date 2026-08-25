#pragma once

#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include "engine/ecs/ecs_structures.h"

/**
 * @brief Util class with general component registry functionality
 */
struct ComponentInfo {
    std::function<void(JsonArchive&, const IComponent&)> serializeFunc;
    std::function<void*(JsonArchive&)> deserializeFunc;
};

class ECSComponentRegistry {
public:
    template <typename T>
    void RegisterComponent(
        std::string type,
        void (*serializeFunc)(JsonArchive&, const T&),
        T (*deserializeFunc)(JsonArchive&)
    ) {
        static_assert(std::is_base_of_v<IComponent, T>, "Registered component type must derive from IComponent");
        if (componentRegistry.find(type) != componentRegistry.end()) {
            throw std::runtime_error("Component type already registered: " + type);
        }
        // create two lambda functions
        componentRegistry[type] = {
            [serializeFunc](JsonArchive& archive, const IComponent& component) {
                serializeFunc(archive, static_cast<const T&>(component));
            },
            [deserializeFunc](JsonArchive& archive) -> void* {
                return new T(deserializeFunc(archive));
            }
        };
    }
    template <typename T>
    const ComponentInfo * GetComponentInfo(std::string type) {
        auto it = componentRegistry.find(type);
        if (it != componentRegistry.end()) {
            return &(it->second);
        }
        return nullptr;
    }
private:
    std::unordered_map<std::string, ComponentInfo> componentRegistry;
};