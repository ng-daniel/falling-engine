#pragma once

#include "engine/ecs/ecs_manager.h"
#include "engine/physics/physics_structures.h"
#include "engine/physics/physics_state.h"

class PhysicsManager {
public:
    PhysicsManager(EcsManager& ecsManager);
    ~PhysicsManager();

    void Update(float dt);

private:
    EcsManager& ecsManager;
    float accumulatedTime = 0.0f; // sits between 0 and PHYS_DT, since it resets after PHYS_DT

    /**
     * @brief Updates the physics simulation for a single tick of length PHYS_DT
     */
    void Tick();
};