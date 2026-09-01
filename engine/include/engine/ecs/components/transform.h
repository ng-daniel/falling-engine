#pragma once

#include "engine/utils/uuid.h"
#include "engine/utils/vector.h"
#include "engine/utils/quaternion.h"
#include "engine/utils/matrix.h"
#include "engine/ecs/ecs_structures.h"

#include "engine/serialization/jsonarchive.h"

/**
 * @brief The transform component
 * Values here are local
 * To get the world value of something, 
 * you need to apply the parent's transform.
 */
struct Transform : public IComponent {
    std::string GetType() const override { return "Transform"; }
    
    Matrix4 matrix;

    // hierarchy references
    UUID parentEntityId = 0;
    UUID firstChildEntityId = 0;
    UUID nextSiblingEntityId = 0;
    UUID prevSiblingEntityId = 0;

    static void Serialize(JsonArchive& archive, const Transform& transform);
    static void Deserialize(JsonArchive& archive, Transform& transform);

    static void SetPosition(Transform& transform, Vector3 newVal);
    static void ChangePosition(Transform& transform, Vector3 diff);

    static void SetRotation(Transform& transform, Quaternion newVal);
    static void ChangeRotation(Transform& transform, Quaternion diff);

    static void SetScale(Transform& transform, Vector3 newVal);
    static void ChangeScale(Transform& transform, Vector3 diff);

    Vector3 GetPosition() const;
    Quaternion GetRotation() const;
    Vector3 GetScale() const;

    static void ComposeTransforms(Transform& result, const Transform& parent, const Transform& child);
};
