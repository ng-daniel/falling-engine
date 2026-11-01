#pragma once

#include "engine/ecs/components/component.h"
#include <glm/vec3.hpp>

struct SphereCollider : public Component {
    std::string GetType() const override { return "SphereCollider"; }

    float radius;
    glm::vec3 relativeOrigin = glm::vec3(0.0f);
};