#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include "engine/ecs/components/component.h"
#include "engine/ecs/ecs_structures.h"

/**
 * @brief Util class with general component registry functionality
 */
struct ComponentInfo {
    std::string type;
    std::function<void(JsonArchive&, const Component&)> serializeFunc;
    std::function<void(JsonArchive&, Component&)> deserializeFunc;
    std::function<void(Entity, Component*)> onCreateFunc;
    std::function<void(Entity, Component*)> onDestroyFunc;
};

class ECSComponentRegistry {
public:
    template <typename T>
    void RegisterSerialization(
        void (*serializeFunc)(JsonArchive&, const T&),
        void (*deserializeFunc)(JsonArchive&, T&)
    ) {
        ComponentInfo& info = EnsureComponentInfo<T>();
        if (info.serializeFunc || info.deserializeFunc) {
            throw std::runtime_error("Serialization already registered: " + info.type);
        }
        info.serializeFunc = [serializeFunc](JsonArchive& archive, const Component& component) {
                serializeFunc(archive, static_cast<const T&>(component));
            };
        info.deserializeFunc = [deserializeFunc](JsonArchive& archive, Component& component) {
                deserializeFunc(archive, static_cast<T&>(component));
            };
    }

    template <typename T>
    void RegisterLifecycle(
        std::function<void(Entity, T*)> onCreate,
        std::function<void(Entity, T*)> onDestroy
    ) {
        ComponentInfo& info = EnsureComponentInfo<T>();
        if ((onCreate || onDestroy) && (info.onCreateFunc || info.onDestroyFunc)) {
            throw std::runtime_error("Lifecycle already registered: " + info.type);
        }
        if (onCreate) {
            info.onCreateFunc = [onCreate = std::move(onCreate)](Entity entity, Component* component) {
                onCreate(entity, static_cast<T*>(component));
            };
        } else {
            info.onCreateFunc = {};
        }
        if (onDestroy) {
            info.onDestroyFunc = [onDestroy = std::move(onDestroy)](Entity entity, Component* component) {
                onDestroy(entity, static_cast<T*>(component));
            };
        } else {
            info.onDestroyFunc = {};
        }
    }

    void OnCreate(Entity entity, Component& component) const {
        if (const ComponentInfo* info = GetComponentInfo(component.GetType()); info && info->onCreateFunc) {
            info->onCreateFunc(entity, &component);
        }
    }

    void OnDestroy(Entity entity, Component& component) const {
        if (const ComponentInfo* info = GetComponentInfo(component.GetType()); info && info->onDestroyFunc) {
            info->onDestroyFunc(entity, &component);
        }
    }
    
    const ComponentInfo * GetComponentInfo(const std::string& type) const {
        auto it = componentRegistry.find(type);
        if (it != componentRegistry.end()) {
            return &(it->second);
        }
        return nullptr;
    }
private:
    template <typename T>
    ComponentInfo& EnsureComponentInfo() {
        static_assert(std::is_base_of_v<Component, T>, "Registered type must derive from Component");
        const std::string type = T{}.GetType();
        auto [it, inserted] = componentRegistry.try_emplace(type);
        if (inserted) {
            it->second.type = type;
        }
        return it->second;
    }

    std::unordered_map<std::string, ComponentInfo> componentRegistry;
};
