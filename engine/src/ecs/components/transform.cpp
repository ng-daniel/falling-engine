#include "engine/ecs/components/transform.h"
#include "engine/serialization/jsonarchive.h"
#include "engine/serialization/vector_serializer.h"
#include "engine/serialization/quaternion_serializer.h"
#include "engine/serialization/matrix_serializer.h"
#include "engine/serialization/uuid_serializer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Transform::Serialize(JsonArchive& archive, const Transform& transform) {
    MatrixSerializer::Serialize(archive, transform.matrix);
    UUIDSerializer::Serialize(archive, transform.parentEntityId, "parentEntityId");
    UUIDSerializer::Serialize(archive, transform.firstChildEntityId, "firstChildEntityId");
    UUIDSerializer::Serialize(archive, transform.nextSiblingEntityId, "nextSiblingEntityId");
    UUIDSerializer::Serialize(archive, transform.prevSiblingEntityId, "prevSiblingEntityId");
}

void Transform::Deserialize(JsonArchive& archive, Transform& transform) {
    MatrixSerializer::Deserialize(archive, transform.matrix);
    UUIDSerializer::Deserialize(archive, transform.parentEntityId, "parentEntityId");
    UUIDSerializer::Deserialize(archive, transform.firstChildEntityId, "firstChildEntityId");
    UUIDSerializer::Deserialize(archive, transform.nextSiblingEntityId, "nextSiblingEntityId");
    UUIDSerializer::Deserialize(archive, transform.prevSiblingEntityId, "prevSiblingEntityId");
}

void Transform::SetPosition(Transform& transform, Vector3 newVal) {
    transform.matrix.SetPosition(newVal);
}

void Transform::ChangePosition(Transform& transform, Vector3 diff) {
    SetPosition(transform, transform.GetPosition() + diff);
}

void Transform::SetRotation(Transform& transform, Quaternion newVal) {
    transform.matrix.SetRotation(newVal);
}

void Transform::ChangeRotation(Transform& transform, Quaternion diff) {
    SetRotation(transform, transform.GetRotation() * diff);
}

void Transform::SetScale(Transform& transform, Vector3 newVal) {
    transform.matrix.SetScale(newVal);
}

void Transform::ChangeScale(Transform& transform, Vector3 diff) {
    SetScale(transform, transform.GetScale() + diff);
}

Vector3 Transform::GetPosition() const { return matrix.GetPosition(); }
Quaternion Transform::GetRotation() const { return matrix.GetRotation(); }
Vector3 Transform::GetScale() const { return matrix.GetScale(); }

/**
 * @brief Produce the world transform into result based on the parent and child transforms.
 *
 * @param result
 * @param parent
 * @param child
 */
void Transform::ComposeTransforms(Transform &result, const Transform &parent, const Transform &child) {
    result.matrix = parent.matrix * child.matrix;
}
