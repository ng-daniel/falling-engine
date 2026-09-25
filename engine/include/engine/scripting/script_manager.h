#pragma once

#include "engine/ecs/entity_handle.h"

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
    void RegisterScript();

    /**
     * @brief Adds a script component of type T to the specified 
     * entity and runs its OnAwake and OnStart callbacks after it is safe to do so.
     * 
     */
    template <typename T>
    void AddScript(EntityHandle owner);

    /**
     * @brief Removes a script component of type T from the
     * specified entity and calls its OnDestroy callback.
     * 
     */
    template <typename T>
    void RemoveScript(EntityHandle owner);

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
     * @brief Updates all registered script types and
     * then applies any deferred operations.
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

    ///
    /// STATIC SCRIPT OPERATIONS
    /// ----------------------------------------------------------------------

    template <typename T>
    static void UpdateScripts(ScriptManager& manager, float deltaTime);

    template <typename T>
    static void AddScriptNow(ScriptManager& manager, EntityHandle owner);

    template <typename T>
    static void RemoveScriptNow(ScriptManager& manager, EntityHandle owner);

    template <typename T>
    static void DestroyScripts(ScriptManager& manager);

    static void RemoveAllScriptsNow(ScriptManager& manager, EntityHandle owner);
    static void DestroyEntityNow(ScriptManager& manager, EntityHandle owner);
    
    /**
     * @brief Executes and flushes all pending deferred script operations.
     * 
     */
    void FlushPendingOperations();

    EcsManager& ecs;
    std::vector<RegisteredScriptType> registeredTypes;
    std::vector<DeferredOp> pendingOperations;
    bool dispatchingCallbacks = false;
    bool shuttingDown = false;
};
