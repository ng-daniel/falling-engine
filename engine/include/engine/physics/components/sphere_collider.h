#pragma once

#include "engine/ecs/components/component.h"
#include "engine/utils/vector.h"

struct SphereCollider : public Component {
    std::string GetType() const override { return "SphereCollider"; }

    float radius;
    Vector3 relativeOrigin;
};