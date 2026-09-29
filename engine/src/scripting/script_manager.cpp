#include "engine/scripting/script_manager.h"

#include <algorithm>

/**
 * @brief Borrows the ECS, which owns all script components.
 */
ScriptManager::ScriptManager(EcsManager& ecs) : ecs(ecs) {}

/**
 * @brief Runs shutdown if the caller did not do so explicitly.
 */
ScriptManager::~ScriptManager() {
    Shutdown();
}

/**
 * @brief Rejects mutations during callbacks or after shutdown.
 * Callback mutations cannot be queued until deferred operations are implemented.
 */
void ScriptManager::CheckCanMutate() const {
    if (dispatchingCallbacks) {
        throw std::logic_error("Script mutations during callbacks require deferred operations");
    }
    if (shuttingDown) {
        throw std::logic_error("ScriptManager has shut down");
    }
}

/**
 * @brief Checks that the handle is live and belongs to this ECS.
 * Matching UUIDs in different ECS instances do not imply ownership.
 */
bool ScriptManager::Owns(EntityHandle owner) const {
    return owner.IsValid() && owner == EntityHandle(ecs, owner.GetUUID());
}

/**
 * @brief Checks the registration list for a script type.
 * Registration order also determines callback dispatch order.
 */
bool ScriptManager::IsRegistered(std::type_index type) const {
    return std::any_of(registeredTypes.begin(), registeredTypes.end(),
                       [type](const RegisteredScriptType& entry) { return entry.type == type; });
}

/**
 * @brief Removes each registered script type from an owned entity.
 * Unregistered component types are outside this manager's lifecycle tracking.
 */
void ScriptManager::RemoveAllScripts(EntityHandle owner) {
    CheckCanMutate();
    if (!Owns(owner)) {
        return;
    }
    for (const RegisteredScriptType& type : registeredTypes) {
        type.remove(*this, owner);
    }
}

/**
 * @brief Destroys scripts before their owning entity.
 * OnDestroy can still resolve the entity through its handle.
 */
void ScriptManager::DestroyEntity(EntityHandle owner) {
    CheckCanMutate();
    if (!Owns(owner)) {
        return;
    }
    RemoveAllScripts(owner);
    owner.Destroy();
}

/**
 * @brief Updates all registered script types under one callback guard.
 * Callbacks cannot change registration or component storage during the pass.
 */
void ScriptManager::Update(float deltaTime) {
    CheckCanMutate();
    CallbackScope callbacks(dispatchingCallbacks);
    for (const RegisteredScriptType& type : registeredTypes) {
        type.update(*this, deltaTime);
    }
}

/**
 * @brief Destroys remaining scripts once and closes the manager.
 * Marks shutdown before OnDestroy to prevent reentry and later mutations.
 */
void ScriptManager::Shutdown() {
    if (shuttingDown) {
        return;
    }
    CheckCanMutate();
    shuttingDown = true;
    for (const RegisteredScriptType& type : registeredTypes) {
        type.destroyAll(*this);
    }
}
