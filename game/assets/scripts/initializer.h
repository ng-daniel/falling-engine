#pragma once

#include "engine/scripting/components/script_data.h"
#include "engine/scripting/script_manager.h"
#include "engine/tools/ecs_extras.h"
#include "engine/utils/random.h"

#include "engine/physics/components/rigidbody.h"
#include "engine/physics/components/sphere_collider.h"

#include "rotator.h"

#include <string>

struct Initializer : ScriptData { 
    UUID modelUUID;
    
    float distance;
    int xSize;
    int ySize;
    int zSize;

    float halfXSize;
    float halfYSize;
    float halfZSize;

    int numToSpawn = 0;
    int totalCount;
    
    std::string GetType() const override { return "Initializer"; }
    
    static void OnCreate(ScriptContext& context, Entity entity, ScriptData& scriptData) {}

    static void OnUpdate(ScriptContext& context, Entity entity, ScriptData& scriptData) {
        Initializer& initializer = static_cast<Initializer&>(scriptData);
        while (initializer.numToSpawn < initializer.totalCount) {
            
            Entity * modelRootEntity = ECSExtras::BuildEntityFromModel(
                context.assets,
                context.ecs,
                initializer.modelUUID
            );
            Entity * root = context.ecs.GetEntity(context.ecs.GetScene());
            context.ecs.Parent(*root, *modelRootEntity);
            Transform  * modelRootTransform = context.ecs.GetComponent<Transform>(*modelRootEntity);
            
            int x = initializer.numToSpawn % initializer.xSize;
            int y = (initializer.numToSpawn / initializer.xSize) % initializer.ySize;
            int z = initializer.numToSpawn / (initializer.xSize * initializer.ySize) - 15;
            Transform::ChangePosition(
                *modelRootTransform,
                glm::vec3(
                    static_cast<float>((x - initializer.halfXSize) * initializer.distance),
                    static_cast<float>((y - initializer.halfYSize) * initializer.distance),
                    static_cast<float>((z - initializer.halfZSize) * initializer.distance)
                )
            );

            Rigidbody * rb = context.ecs.AddComponent<Rigidbody>(*modelRootEntity);
            // rb->hasGravity = false;
            SphereCollider * sphereCollider = context.ecs.AddComponent<SphereCollider>(*modelRootEntity);

            // apply an impulse in a random upwards direction
            glm::vec3 impulse = glm::vec3(
                Random::RandFloat(-1.0f, 1.0f),
                Random::RandFloat(0.5f, 1.0f),
                Random::RandFloat(-1.0f, 1.0f)
            );
            Rigidbody::AddImpulse(*rb, impulse * 10.0f);

            if (!rb->hasGravity) {
                Rotator * rotator = context.scripts.AddScript<Rotator>(*modelRootEntity);
                Rotator::SetSpeed(*rotator, Random::RandFloat(10.0f, 20.0f));
                Rotator::SetHoverOscillationSpeed(*rotator, 1.0f);
                Rotator::SetHoverOscillationAmplitude(*rotator, 0.5f);
                Rotator::SetRotationAxis(*rotator, glm::vec3(0.0f, 1.0f, 0.0f));
                Rotator::SetInitialTimer(*rotator, float(x + z + y) / 2.0f);
            }
            
            initializer.numToSpawn++;
        }
    }

    static void Initialize(
        ScriptData& scriptData,
        int x, int y, int z, float dist,
        UUID modelUUID)
    {
        Initializer& initializer = static_cast<Initializer&>(scriptData);
        initializer.modelUUID = modelUUID;
        initializer.distance = dist;
        initializer.xSize = x;
        initializer.ySize = y;
        initializer.zSize = z;

        initializer.halfXSize = initializer.xSize / 2.0f;
        initializer.halfYSize = initializer.ySize / 2.0f;
        initializer.halfZSize = initializer.zSize / 2.0f;

        initializer.numToSpawn = 0;
        initializer.totalCount = initializer.xSize * initializer.ySize * initializer.zSize;
    }
};