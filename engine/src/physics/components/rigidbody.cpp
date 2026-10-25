#include "engine/physics/components/rigidbody.h"
#include "engine/utils/vector.h"

void Rigidbody::AddImpulse(Rigidbody &body, Vector3 impulse) {
    Vector3 dv = impulse / body.mass;
    body.velocity = body.velocity + dv;
}

void Rigidbody::AddForce(Rigidbody &body, Vector3 force) {
    body.forceAccumulation = body.forceAccumulation + force;
}

void Rigidbody::ResolveForces(Rigidbody &body, float dt) {
    Vector3 a = body.forceAccumulation / body.mass;
    Vector3 dv  = 0.5f * a * dt * dt;
    body.velocity = body.velocity + dv;
}

