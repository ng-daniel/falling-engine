#include "engine/physics/physics_manager.h"
#include "engine/physics/components/rigidbody.h"

PhysicsManager::PhysicsManager(EcsManager& ecsManager)
    : ecsManager(ecsManager) {
}

PhysicsManager::~PhysicsManager() {
}

void PhysicsManager::Update(float dt) {
    accumulatedTime += dt;
    // while loop to account for frames exceeding 2 * PHYS_DT
    while (accumulatedTime >= PHYS_DT) {
        Tick();
        accumulatedTime -= PHYS_DT;
    }
}

void PhysicsManager::Tick() {
    EntityComponentView view = ecsManager.GetEntityComponentView<Rigidbody>();
    for (auto [entityId, rb] : view) {
        // apply final gravity force to the rigidbody
        if (rb.hasGravity) {
            Rigidbody::AddForce(rb, {0.0f, -PHYS_GRAVITY * rb.mass, 0.0f});
        }
        Rigidbody::ResolveForces(rb, PHYS_DT);
        
        // compute and apply position change based on velocity and time step
        Vector3 dp = rb.velocity * PHYS_DT;
        Transform::ChangePosition(
            *ecsManager.GetComponent<Transform>(
                *ecsManager.GetEntity(entityId)
            ),
            dp
        );
    }
}