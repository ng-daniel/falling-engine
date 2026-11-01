#pragma once

#include "engine/ecs/components/component.h"
#include <glm/vec3.hpp>

struct Rigidbody : public Component {
public:
    std::string GetType() const override { return "Rigidbody"; }

    float mass = 1.0f;
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 forceAccumulation = glm::vec3(0.0f, 0.0f, 0.0f);
    bool hasGravity = true;

    /**
     * @brief Adds to the accumulated force over the course of a physics tick,
     * total accumulated force is evaluated all at once at the end of each tick. 
     */
    static void AddForce(Rigidbody& body, glm::vec3 force);

    /**
     * @brief Immediately applies the change in velocity caused by the
     * provided impulse.
     */
    static void AddImpulse(Rigidbody& body, glm::vec3 impulse);

    /**
     * @brief Computes and applies the net velocity change based on the accumulated forces
     * over the course of the physics timestep.
     */
     static void ResolveForces(Rigidbody& body, float dt);
};