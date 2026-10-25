#pragma once

#include "engine/ecs/components/component.h"
#include "engine/utils/vector.h"

struct Rigidbody : Component {
public:
    float mass;
    Vector3 velocity;
    Vector3 forceAccumulation;
    
    /**
     * @brief Adds to the accumulated force over the course of a physics tick,
     * total accumulated force is evaluated all at once at the end of each tick. 
     */
    static void AddForce(Rigidbody& body, Vector3 force);

    /**
     * @brief Immediately applies the change in velocity caused by the
     * provided impulse.
     */
    static void AddImpulse(Rigidbody& body, Vector3 impulse);

    /**
     * @brief Computes and applies the net velocity change based on the accumulated forces
     * over the course of the physics timestep.
     */
     static void ResolveForces(Rigidbody& body, float dt);
};