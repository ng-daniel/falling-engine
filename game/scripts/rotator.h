#include "engine/scripting/components/script_data.h"
#include "engine/scripting/script_manager.h"
#include "engine/ecs/components/transform.h"
#include "engine/utils/quaternion.h"
#include "engine/utils/time.h"
#include <string>

struct Rotator : ScriptData {
    float speed;
    
    std::string GetType() const override { return "Rotator"; }

    static void RotateAmount(Transform& transform, float amount) {
        Transform::ChangeRotation(
            transform,
            Quaternion::EulerToQuaternion(
                amount,
                0.0f,
                0.0f
            )
        );
    }
    
    static void OnCreate(ScriptContext& context, Entity entity, ScriptData& scriptData) {
        Rotator& rotator = static_cast<Rotator&>(scriptData);
        rotator.speed = 1.0f; // Default rotation speed

    }

    static void OnUpdate(ScriptContext& context, Entity entity, ScriptData& scriptData) {
        Rotator& rotator = static_cast<Rotator&>(scriptData);
        Transform * transform = context.GetComponent<Transform>(entity);
        Rotator::RotateAmount(*transform, rotator.speed * Time::GetDeltaTime());
    }

    static void SetSpeed(ScriptData& scriptData, float newSpeed) {
        Rotator& rotator = static_cast<Rotator&>(scriptData);
        rotator.speed = newSpeed;
    }
};