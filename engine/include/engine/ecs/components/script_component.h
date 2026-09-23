#pragma once

#include "engine/ecs/components/component.h"
#include "engine/scripting/script_handle.h"

#include <vector>

class ScriptManager;

/**
 * @brief Holds the handles of scripts attached to an entity.
 * @details ScriptManager owns the actual scripts and updates this list.
 * Copying or moving this component only copies or moves the handles, so normal
 * ECS swap removal does not destroy script instances or transfer their ownership.
 */
struct ScriptComponent : public Component {
    std::string GetType() const override { return "ScriptComponent"; }

    const std::vector<ScriptHandle>& GetScripts() const { return scripts; }

private:
    friend class ScriptManager;

    std::vector<ScriptHandle> scripts;
};
