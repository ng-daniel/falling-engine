#pragma once

#include "engine/ecs/entity_handle.h"
#include "engine/scripting/script.h"

#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <vector>

/**
 * @brief Dispatches lifecycle callbacks for scripts stored as ECS components.
 * @details Because ECS is designed for generic components, we need a special
 * interface to manage script lifecycles such as awake, start, update, and destroy
 */
class ScriptManager {
public:
    explicit ScriptManager(EcsManager& ecs);
    ~ScriptManager();

    ScriptManager(const ScriptManager&) = delete;
    ScriptManager& operator=(const ScriptManager&) = delete;
    ScriptManager(ScriptManager&&) = delete;
    ScriptManager& operator=(ScriptManager&&) = delete;

    /**
     * @brief Registers a concrete script component type once for frame updates and cleanup.
     * 
     */
    template <typename T>
    void RegisterScript() {
        static_assert(std::is_base_of_v<Script, T>, "Script types must derive from Script");
        CheckCanMutate();
        if (IsRegistered(typeid(T))) {
            return;
        }
        registeredTypes.push_back({typeid(T), &UpdateScripts<T>, &RemoveScriptNow<T>, &DestroyScripts<T>});
    }

    /**
     * @brief Adds a script component of type T to the specified 
     * entity and runs its OnAwake and OnStart callbacks after it is safe to do so.
     * 
     */
    template <typename T>
    void AddScript(EntityHandle owner) {
        static_assert(std::is_base_of_v<Script, T>, "Script types must derive from Script");
        CheckCanMutate();
        if (!IsRegistered(typeid(T))) {
            throw std::logic_error("Register the script type before adding it");
        }
        if (Owns(owner)) {
            AddScriptNow<T>(*this, owner);
        }
    }

    /**
     * @brief Removes a script component of type T from the
     * specified entity and calls its OnDestroy callback.
     * 
     */
    template <typename T>
    void RemoveScript(EntityHandle owner) {
        static_assert(std::is_base_of_v<Script, T>, "Script types must derive from Script");
        CheckCanMutate();
        if (Owns(owner)) {
            RemoveScriptNow<T>(*this, owner);
        }
    }

    /**
     * @brief Removes all script components from the specified
     * entity and calls their OnDestroy callbacks.
     * 
     */
    void RemoveAllScripts(EntityHandle owner);

    /**
     * @brief Destroys all script components attached to the specified
     * entity and calls their OnDestroy callbacks before the entity itself is destroyed.
     * 
     */
    void DestroyEntity(EntityHandle owner);

    /**
     * @brief Updates all registered script types.
     * Deferred operations will be added after the immediate workflow is reviewed.
     * 
     */
    void Update(float deltaTime);

    /**
     * @brief Shuts down the script manager by calling 
     * OnDestroy for all remaining scripts and preventing further operations.
     * 
     */
    void Shutdown();

private:
    
    /**
     * @brief since the add and remove operations have the same signature
     * we can define a common type alias for them
     * they are evil because they modify things unsafely if done within the update cycle
     * 
     */
    using EvilScriptOp = void (*)(ScriptManager&, EntityHandle);

    /**
     * @brief Represents a script component type that has been registered with the script manager.
     * 
     */
    struct RegisteredScriptType {
        std::type_index type;
        void (*update)(ScriptManager&, float);
        EvilScriptOp remove;
        void (*destroyAll)(ScriptManager&);
    };

    /**
     * @brief Nice bundle containing all the information needed
     * for deferred script operations
     */
    struct DeferredOp {
        EvilScriptOp operation; // the operation that got deferred
        EntityHandle owner;
    };

    /**
     * @brief Marks script callbacks as active so CheckCanMutate rejects changes to ECS
     * storage while callbacks run. The destructor clears the flag even if a
     * callback throws.
     * @note This is used internally by the script manager to ensure safe mutation of ECS storage.
     * Use this when you do script ops that might modify ECS storage
     * sets the "dispatchingCallbacks" flag and unsets it when the scope is destroyed
     */
    struct CallbackScope {
        explicit CallbackScope(bool& flag) : flag(flag) { flag = true; }
        ~CallbackScope() { flag = false; }
        bool& flag;
    };

    /**
     * @brief Updates every script of one registered type.
     * Update owns the callback guard, so dense storage cannot change during iteration.
     */
    template <typename T>
    static void UpdateScripts(ScriptManager& manager, float deltaTime) {
        auto view = manager.ecs.GetEntityComponentView<T>();
        for (std::size_t index = 0; index < view.size(); ++index) {
            view.components[index].OnUpdate(deltaTime);
        }
    }

    /**
     * @brief Adds a script and runs its initialization callbacks.
     * Assigns the owner before OnAwake so GetCore is valid there.
     */
    template <typename T>
    static void AddScriptNow(ScriptManager& manager, EntityHandle owner) {
        if (owner.HasComponent<T>()) {
            return;
        }
        T* script = owner.AddComponent<T>();
        if (!script) {
            return;
        }
        script->core = owner;
        CallbackScope callbacks(manager.dispatchingCallbacks);
        script->OnAwake();
        script->OnStart();
    }

    /**
     * @brief Calls OnDestroy before removing the component.
     * The callback scope ends before ECS storage changes.
     */
    template <typename T>
    static void RemoveScriptNow(ScriptManager& manager, EntityHandle owner) {
        T* script = owner.GetComponent<T>();
        if (!script) {
            return;
        }
        {
            CallbackScope callbacks(manager.dispatchingCallbacks);
            script->OnDestroy();
        }
        owner.RemoveComponent<T>();
    }

    /**
     * @brief Destroys all scripts of one registered type during shutdown.
     * Calls OnDestroy before removal, then refreshes the view because dense
     * storage swaps entries.
     */
    template <typename T>
    static void DestroyScripts(ScriptManager& manager) {
        auto view = manager.ecs.GetEntityComponentView<T>();
        {
            CallbackScope callbacks(manager.dispatchingCallbacks);
            for (std::size_t index = 0; index < view.size(); ++index) {
                view.components[index].OnDestroy();
            }
        }
        auto remaining = manager.ecs.GetEntityComponentView<T>();
        while (!remaining.empty()) {
            Entity* entity = manager.ecs.GetEntity(remaining.entityRuntimeIds[0]);
            manager.ecs.RemoveComponent<T>(*entity);
            remaining = manager.ecs.GetEntityComponentView<T>();
        }
    }

    /**
     * @brief Executes and flushes all pending deferred script operations.
     * 
     */
    void FlushPendingOperations();

    void CheckCanMutate() const;
    bool Owns(EntityHandle owner) const;
    bool IsRegistered(std::type_index type) const;

    EcsManager& ecs;
    std::vector<RegisteredScriptType> registeredTypes;
    std::vector<DeferredOp> pendingOperations;
    bool dispatchingCallbacks = false;
    bool shuttingDown = false;
};
