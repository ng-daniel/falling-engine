#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <utility>
#include <vector>

#include "engine/ecs/ecs_manager.h"
#include "engine/scripting/script_runtime.h"

/**
 * @brief Holds the context for script operations, including access to the ECS manager.
 * 
 */
struct ScriptContext {
    EcsManager& ecs;
};

/**
 * @brief Stores the runtime and update callback for a specific script type.
 * 
 */
struct ScriptRegistryInfo {
    std::type_index type;
    std::unique_ptr<ScriptRuntime> runtime;
    std::function<void(ScriptContext&, ScriptRuntime&)> updateCallback;
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
     * @brief Called to register a script type and its associated runtime.
     * Only call once per script type.
     * 
     * @tparam T 
     * @tparam TRuntime 
     */
    template <typename T, typename TRuntime>
    bool RegisterScript() {
        static_assert(std::is_base_of_v<ScriptData, T>, "Script data must derive from ScriptData");
        static_assert(std::is_base_of_v<ScriptRuntime, TRuntime>, "Script runtime must derive from ScriptRuntime");

        if (IsTypeAlreadyRegistered(typeid(T))) {
            return false;
        }

        // allocate space for the new registration
        // and the script runtime class
        registrations.reserve(registrations.size() + 1);
        auto runtime = std::make_unique<TRuntime>();
        ScriptRuntime * runtimePtr = runtime.get();
        
        context.ecs.GetComponentRegistry().RegisterLifecycle<T>(
            [this, runtimePtr](Entity entity, T* data) {
                runtimePtr->OnCreate(context, entity, *data);
            },
            [this, runtimePtr](Entity entity, T* data) {
                runtimePtr->OnDestroy(context, entity, *data);
            }
        );

        ScriptRegistryInfo reg = {
            typeid(T), std::move(runtime),
            [](ScriptContext& context, ScriptRuntime& runtime) {
                for (auto [runtimeId, data] : context.ecs.GetEntityComponentView<T>()) {
                    if (Entity* entity = context.ecs.GetEntity(runtimeId)) {
                        runtime.OnUpdate(context, *entity, data);
                    }
                }
            }
        };
        registrations.push_back(std::move(reg));
        return true;
    }

    /**
     * @brief Calls the update callback for every registered script type
     */
    void UpdateAllScripts() {
        for (const ScriptRegistryInfo& registration : registrations) {
            registration.updateCallback(context, *registration.runtime);
        }
    }

private:
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
