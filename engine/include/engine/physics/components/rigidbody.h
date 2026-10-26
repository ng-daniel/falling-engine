#pragma once

#include "engine/ecs/components/component.h"
#include "engine/utils/vector.h"

struct Rigidbody : public Component {
public:
    std::string GetType() const override { return "Rigidbody"; }

    float mass = 1.0f;
    Vector3 velocity = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 forceAccumulation = Vector3(0.0f, 0.0f, 0.0f);
    bool hasGravity = true;

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