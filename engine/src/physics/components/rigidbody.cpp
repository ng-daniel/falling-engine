#include "engine/physics/components/rigidbody.h"
#include <glm/vec3.hpp>

void Rigidbody::AddImpulse(Rigidbody &body, glm::vec3 impulse) {
    glm::vec3 dv = impulse / body.mass;
    body.velocity = body.velocity + dv;
}

void Rigidbody::AddForce(Rigidbody &body, glm::vec3 force) {
    body.forceAccumulation = body.forceAccumulation + force;
}

void Rigidbody::ResolveForces(Rigidbody &body, float dt) {
    glm::vec3 a = body.forceAccumulation / body.mass;
    glm::vec3 dv  = a * dt;
    body.velocity = body.velocity + dv;
    body.forceAccumulation = glm::vec3(0.0f, 0.0f, 0.0f);
}

