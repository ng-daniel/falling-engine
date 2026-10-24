#pragma once

#include "engine/ecs/components/component.h"
#include "engine/utils/vector.h"

struct RigidBody : Component {
private:
    float mass;
    Vector3 velocity;
public:
    const float Mass() const { return mass; }
    const Vector3 Velocity() const { return velocity; }
    
    void AddForce(Vector3 force);
};