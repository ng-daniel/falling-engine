#include "engine/scripting/components/script_data.h"
#include "engine/scripting/script_manager.h"
#include "engine/ecs/components/transform.h"
#include "engine/utils/quaternion.h"
#include "engine/utils/time.h"
#include <string>
#include "engine/debug/logger.h"

struct Rotator : ScriptData {
    float speed;
    float hoverOscillationSpeed;
    float hoverOscillationAmplitude;
    float timer;
    Vector3 initialPosition;
    Vector3 rotationAxis;
    
    std::string GetType() const override { return "Rotator"; }

    static void RotateAmount(Transform& transform, float amount, const Vector3& axis) {
        // use quaternion math to rotate the transform by the given amount around the specified axis
        Quaternion currentRotation = transform.GetRotation();
        Quaternion deltaRotation = Quaternion::AngleAxis(amount, axis);
        Quaternion newRotation = deltaRotation * currentRotation;
        
        Transform::SetRotation(
            transform,
            newRotation
        );
    }
    
    static void OnCreate(ScriptContext& context, Entity entity, ScriptData& scriptData) {
        Rotator& rotator = static_cast<Rotator&>(scriptData);
        rotator.speed = 1.0f; // Default rotation speed
        rotator.hoverOscillationSpeed = 1.0f; // Default hover oscillation speed
        rotator.hoverOscillationAmplitude = 1.0f; // Default hover oscillation amplitude
        rotator.timer = 0.0f; // Initialize timer
        rotator.initialPosition = context.GetComponent<Transform>(entity)->GetPosition();
        rotator.rotationAxis = Vector3(0.0f, 1.0f, 0.0f); // Default rotation axis
    }

    static void OnUpdate(ScriptContext& context, Entity entity, ScriptData& scriptData) {
        Rotator& rotator = static_cast<Rotator&>(scriptData);
        Transform * transform = context.GetComponent<Transform>(entity);
        Rotator::RotateAmount(*transform, rotator.speed * Time::GetDeltaTime(), rotator.rotationAxis);
        float hoverOffset = std::sin(
            rotator.timer * rotator.hoverOscillationSpeed
        ) * rotator.hoverOscillationAmplitude;
        if (entity.entityRuntimeIdx == 101) {
            // Logger::Info("rotator", "Hover offset: " + std::to_string(hoverOffset));
        }
        Transform::SetPosition(
            *transform,
            Vector3(
                rotator.initialPosition.x,
                rotator.initialPosition.y + hoverOffset,
                rotator.initialPosition.z
            )
        );
        rotator.timer += Time::GetDeltaTime();
    }

    static void SetSpeed(ScriptData& scriptData, float newSpeed) {
        Rotator& rotator = static_cast<Rotator&>(scriptData);
        rotator.speed = newSpeed;
    }

    static void SetHoverOscillationSpeed(ScriptData& scriptData, float newSpeed) {
        Rotator& rotator = static_cast<Rotator&>(scriptData);
        rotator.hoverOscillationSpeed = newSpeed;
    }

    static void SetHoverOscillationAmplitude(ScriptData& scriptData, float newAmplitude) {
        Rotator& rotator = static_cast<Rotator&>(scriptData);
        rotator.hoverOscillationAmplitude = newAmplitude;
    }

    static void SetRotationAxis(ScriptData& scriptData, const Vector3& newAxis) {
        Rotator& rotator = static_cast<Rotator&>(scriptData);
        rotator.rotationAxis = newAxis;
    }

    static void SetInitialTimer(ScriptData& scriptData, float initialTimer) {
        Rotator& rotator = static_cast<Rotator&>(scriptData);
        rotator.timer = initialTimer;
    }
};