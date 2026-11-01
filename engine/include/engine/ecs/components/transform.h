#pragma once

#include "engine/utils/uuid.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include "engine/utils/transform_utils.h"
#include "engine/ecs/components/component.h"

/**
 * @brief The transform component
 * Values here are local
 * To get the world value of something, 
 * you need to apply the parent's transform.
 */
struct Transform : public Component {
    std::string GetType() const override { return "Transform"; }
    
    glm::mat4 matrix = glm::mat4(1.0f);

    // hierarchy references
    UUID parentEntityId = 0;
    UUID firstChildEntityId = 0;
    UUID nextSiblingEntityId = 0;
    UUID prevSiblingEntityId = 0;

    static void SetPosition(Transform& transform, glm::vec3 newVal);
    static void ChangePosition(Transform& transform, glm::vec3 diff);

    static void SetRotation(Transform& transform, glm::quat newVal);
    static void ChangeRotation(Transform& transform, glm::quat diff);

    static void SetScale(Transform& transform, glm::vec3 newVal);
    static void ChangeScale(Transform& transform, glm::vec3 diff);

    glm::vec3 GetPosition() const;
    glm::quat GetRotation() const;
    glm::vec3 GetScale() const;
    TransformDecomposition GetDecomposition() const;

    static glm::mat4 FromTRS(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
    static glm::mat4 ComposeTransforms(const glm::mat4 &parentTransform, const glm::mat4 &childTransform);
};

class TransformSerializer : public ComponentSerializer {
public:
    static void Serialize(JsonArchive& archive, const Transform& transform);
    static void Deserialize(JsonArchive& archive, Transform& transform);
};
