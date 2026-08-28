#pragma once

#include "engine/utils/uuid.h"
#include "engine/utils/vector.h"
#include "engine/ecs/ecs_structures.h"

#include "engine/serialization/jsonarchive.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief The transform component
 * Values here are local
 * To get the world value of something, 
 * you need to apply the parent's transform.
 */
struct Transform : public IComponent {
    std::string GetType() const override { return "Transform"; }
    
    Vector3 position;
    Vector4 rotation;
    Vector3 scale;

    // hierarchy references
    UUID parentEntityId;
    UUID firstChildEntityId;
    UUID nextSiblingEntityId;
    UUID prevSiblingEntityId;

    static void Serialize(JsonArchive& archive, const Transform& transform);
    static void Deserialize(JsonArchive& archive, Transform& transform);

    static void SetPosition(Transform& transform, Vector3 newVal);
    static void ChangePosition(Transform& transform, Vector3 diff);

    static void SetRotation(Transform& transform, Vector4 newVal);
    static void ChangeRotation(Transform& transform, Vector4 diff);

    static void SetScale(Transform& transform, Vector3 newVal);
    static void ChangeScale(Transform& transform, Vector3 diff);
};