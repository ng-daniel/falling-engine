#pragma once

#include "engine/ecs/ecs_structures.h"
#include "engine/scripting/components/script_data.h"

struct ScriptContext;

/**
 * @brief Base class for script runtimes.
 * 
 * One runtime handles every ScriptData instance of its registered type.
 */
class ScriptRuntime {
public:
    virtual ~ScriptRuntime() = default;

    virtual void OnCreate(ScriptContext&, Entity, ScriptData&) {}
    virtual void OnUpdate(ScriptContext&, Entity, ScriptData&) {}
    virtual void OnDestroy(ScriptContext&, Entity, ScriptData&) {}
};
