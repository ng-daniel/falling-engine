#pragma once

#include "engine/ecs/components/component.h"
#include "engine/ecs/ecs_structures.h"

struct ScriptContext;

/**
 * @brief Base class for ECS-owned script data components.
 * 
 * Derived types store per-entity state and may hide these no-op callbacks
 * with static functions taking (ScriptContext&, Entity, Derived&).
 */
struct ScriptData : Component {
    static void OnCreate(ScriptContext&, Entity, ScriptData&) {}
    static void OnUpdate(ScriptContext&, Entity, ScriptData&) {}
    static void OnDestroy(ScriptContext&, Entity, ScriptData&) {}
};
