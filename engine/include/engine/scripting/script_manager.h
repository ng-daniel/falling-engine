#pragma once

#include "engine/ecs/entity_handle.h"
#include "engine/scripting/script.h"
#include "engine/scripting/script_handle.h"

/**
 * @brief Owns scripts and manages their creation, initialization, updates, and removal.
 * @details Each manager belongs to one ECS. Entities only store script handles;
 * the manager is responsible for the instances themselves.
 * This declares the API for the scripting module. Storage and lifecycle method
 * definitions will be added in the ownership and deferred operations steps.
 */
class ScriptManager {
public:
    explicit ScriptManager(EcsManager& ecs);
    ~ScriptManager();

    ScriptManager(const ScriptManager&) = delete;
    ScriptManager& operator=(const ScriptManager&) = delete;
    ScriptManager(ScriptManager&&) = delete;
    ScriptManager& operator=(ScriptManager&&) = delete;

    /// SCRIPT LIFECYCLE
    /// ----------------------------------------------

    /**
     * @brief Creates a script and attaches it to its owning entity.
     * @tparam T A class derived from Script.
     * @param owner An entity in this manager's ECS.
     * @return The script handle, or an empty handle if the owner is invalid.
     * @details Adds ScriptComponent when needed. During callbacks, initialization
     * waits until iteration finishes and the script first updates next frame.
     */
    template <typename T>
    ScriptHandle AddScript(EntityHandle core);

    /**
     * @brief Removes a script created by this manager.
     * Empty or already removed handles do nothing. During callbacks, destruction
     * waits until iteration finishes.
     */
    void RemoveScript(ScriptHandle scriptHandle);

    /**
     * @brief Removes all scripts attached to the given entity.
     */
    void RemoveAllScripts(EntityHandle core);

    /**
     * @brief Updates active scripts, then applies pending lifecycle operations.
     * @param deltaTime Time since the previous frame, in seconds.
     */
    void Update(float deltaTime);

    /**
     * @brief Destroys all scripts and stops accepting new attachments.
     * Calling this more than once is safe. The ECS must still exist during cleanup.
     */
    void Shutdown();
};
