#pragma once

#include <algorithm>
#include <typeindex>
#include <type_traits>
#include <utility>
#include <vector>

#include "engine/ecs/ecs_manager.h"
#include "engine/scripting/components/script_data.h"

/**
 * @brief Holds the context for script operations, including access to the ECS manager.
 * 
 */
struct ScriptContext {
    EcsManager& ecs;

    /**
     * @brief Gets a component from an entity supplied to a script callback.
     * @return The component, or nullptr if the entity does not have it.
     */
    template <typename T>
    T* GetComponent(Entity entity) {
        return ecs.GetComponent<T>(entity);
    }

    /**
     * @brief Resolves an entity UUID and gets its current component.
     * @return The component, or nullptr if the entity or component is missing.
     */
    template <typename T>
    T* GetComponent(UUID entityId) {
        Entity* entity = ecs.GetEntity(entityId);
        return entity ? ecs.GetComponent<T>(*entity) : nullptr;
    }
};

/**
 * @brief Stores the update callback for a specific script type.
 * 
 */
struct ScriptRegistryInfo {
    std::type_index type;
    void (*updateCallback)(ScriptContext&);
};

/**
 * @brief Responsible for a couple things:
 * 1. When a new script is created, register it in the list
 */
class ScriptManager {
public:
    explicit ScriptManager(EcsManager& ecs) : context{ecs} {}
    ~ScriptManager() {}
    ScriptManager(const ScriptManager&) = delete;
    ScriptManager& operator=(const ScriptManager&) = delete;

    /**
     * @brief Adds script data and registers its behavior on first use.
     * @tparam T Script data type with static lifecycle callbacks.
     * @param entity Entity that owns the script data.
     * @param initialValue Initial script data.
     * @return The added script data, or nullptr if it could not be added.
     */
    template <typename T>
    T* AddScript(Entity entity, T initialValue = T{}) {
        if (!IsTypeAlreadyRegistered(typeid(T))) {
            RegisterScript<T>();
        }
        return context.ecs.AddComponent<T>(entity, std::move(initialValue));
    }

    /**
     * @brief Returns script data through the ECS manager.
     */
    template <typename T>
    T* GetScript(Entity entity) {
        return context.ecs.GetComponent<T>(entity);
    }

    /**
     * @brief Removes script data through the ECS manager.
     * 
     */
    template <typename T>
    void RemoveScript(Entity entity) {
        context.ecs.RemoveComponent<T>(entity);
    }

    /**
     * @brief Registers a script component's static lifecycle callbacks.
     * Only call once per script type.
     * 
     * @tparam T Script data component type.
     */
    template <typename T>
    bool RegisterScript() {
        static_assert(std::is_base_of_v<ScriptData, T>, "Script data must derive from ScriptData");

        if (IsTypeAlreadyRegistered(typeid(T))) {
            return false;
        }

        context.ecs.GetComponentRegistry().RegisterLifecycle<T>(
            [this](Entity entity, T* data) {
                T::OnCreate(context, entity, *data);
            },
            [this](Entity entity, T* data) {
                T::OnDestroy(context, entity, *data);
            }
        );

        registrations.push_back({typeid(T), &UpdateScriptType<T>});
        return true;
    }

    /**
     * @brief Calls the update callback for every registered script type
     */
    void UpdateAllScripts() {
        for (const ScriptRegistryInfo& registration : registrations) {
            registration.updateCallback(context);
        }
    }

private:
    template <typename T>
    static void UpdateScriptType(ScriptContext& context) {
        for (auto [runtimeId, data] : context.ecs.GetEntityComponentView<T>()) {
            if (Entity* entity = context.ecs.GetEntity(runtimeId)) {
                T::OnUpdate(context, *entity, data);
            }
        }
    }

    bool IsTypeAlreadyRegistered(const std::type_index& type) {
        bool typeExists = std::any_of(
            registrations.begin(),
            registrations.end(),
            [type](const ScriptRegistryInfo& registration) { return registration.type == type; }
        );
        return typeExists;
    }

    ScriptContext context;
    std::vector<ScriptRegistryInfo> registrations;
};
