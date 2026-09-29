#pragma once

#include "engine/ecs/entity_handle.h"
#include "engine/ecs/components/component.h"

class ScriptManager;

/**
 * @brief Base class for game behavior attached to an entity.
 * @details The ECS owns the instance; ScriptManager assigns its entity before OnAwake.
 * Component access goes through EntityHandle, which also holds the ECS reference.
 * Components can move when arrays grow or swap-remove entries, so scripts should
 * keep handles and fetch components when needed instead of storing their pointers.
 */
class Script : public Component {
public:
    virtual ~Script() = default;

    std::string GetType() const override { return "Script"; }

    EntityHandle GetCore() const { return core; }

    /// LIFECYCLE
    /// ----------------------------------------------

    /**
     * @brief Called once when the manager initializes this script.
     * The owning entity is assigned before this callback, not in the constructor.
     */
    virtual void OnAwake() {}

    /**
     * @brief Called once after OnAwake and before the first OnUpdate.
     * Use this for initialization that depends on other components being awake.
     */
    virtual void OnStart() {}

    /**
     * @brief Called each frame while the script is active.
     * @param deltaTime Time since the previous frame, in seconds.
     */
    virtual void OnUpdate(float deltaTime) {}

    /**
     * @brief Called before an initialized script is removed.
     * The owning entity may already be destroyed, so component access can fail.
     */
    virtual void OnDestroy() {}

protected:
    Script() = default;

private:
    friend class ScriptManager;
    EntityHandle core;
};
