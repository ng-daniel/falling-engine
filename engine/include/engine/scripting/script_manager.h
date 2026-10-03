#pragma once

#include <algorithm>
#include <stdexcept>
#include <typeindex>
#include <type_traits>
#include <vector>

#include "engine/ecs/ecs_manager.h"

struct ScriptContext {
    EcsManager& ecs;
};

/**
 * @brief Manages the registration and lifecycle of
 * script components within the ECS.
 * 
 */
class ScriptManager {
public:
    explicit ScriptManager(EcsManager& ecs) : context{ecs} {}
    ~ScriptManager() {
        for (const Registration& registration : registrations) {
            registration.DestroyAll(context);
            registration.ClearLifecycle(context.ecs);
        }
    }

    ScriptManager(const ScriptManager&) = delete;
    ScriptManager& operator=(const ScriptManager&) = delete;

    template <typename T>
    void RegisterScript() {
        static_assert(std::is_base_of_v<Component, T>, "Script must be an ECS component");

        const std::type_index type(typeid(T));
        if (std::any_of(registrations.begin(), registrations.end(),
                [type](const Registration& registration) { return registration.type == type; })) {
            throw std::runtime_error("Script type already registered");
        }

        context.ecs.GetComponentRegistry().RegisterLifecycle<T>(
            [this](Entity entity) {
                if (T* script = context.ecs.GetComponent<T>(entity)) {
                    T::OnCreate(context, entity, *script);
                }
            },
            [this](Entity entity) {
                if (T* script = context.ecs.GetComponent<T>(entity)) {
                    T::OnDestroy(context, entity, *script);
                }
            }
        );

        registrations.push_back({
            type,
            [](ScriptContext& context, float dt) {
                for (auto [runtimeId, script] : context.ecs.GetEntityComponentView<T>()) {
                    if (Entity* entity = context.ecs.GetEntity(runtimeId)) {
                        T::OnUpdate(context, *entity, script, dt);
                    }
                }
            },
            [](ScriptContext& context) {
                for (auto [runtimeId, script] : context.ecs.GetEntityComponentView<T>()) {
                    if (Entity* entity = context.ecs.GetEntity(runtimeId)) {
                        T::OnDestroy(context, *entity, script);
                    }
                }
            },
            [](EcsManager& ecs) { ecs.GetComponentRegistry().RegisterLifecycle<T>({}, {}); }
        });

        // Components added before registration receive their creation callback.
        for (auto [runtimeId, script] : context.ecs.GetEntityComponentView<T>()) {
            if (Entity* entity = context.ecs.GetEntity(runtimeId)) {
                T::OnCreate(context, *entity, script);
            }
        }
    }

    void Update(float dt) {
        for (const Registration& registration : registrations) {
            registration.Update(context, dt);
        }
    }

private:
    struct Registration {
        std::type_index type;
        void (*Update)(ScriptContext&, float);
        void (*DestroyAll)(ScriptContext&);
        void (*ClearLifecycle)(EcsManager&);
    };

    ScriptContext context;
    std::vector<Registration> registrations;
};
