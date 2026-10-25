#pragma once

#include "engine/ecs/components/component.h"
#include "engine/utils/vector.h"

struct SphereCollider : Component {
    float radius;
    Vector3 relOrigin;
};