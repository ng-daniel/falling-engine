#include "engine/ecs/components/transform.h"
#include "engine/serialization/jsonarchive.h"
#include "engine/serialization/matrix_serializer.h"
#include "engine/serialization/uuid_serializer.h"

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
    transform.matrix(0, 3) = newVal.x;
    transform.matrix(1, 3) = newVal.y;
    transform.matrix(2, 3) = newVal.z;
}

void Transform::ChangePosition(Transform& transform, Vector3 diff) {
    SetPosition(transform, transform.GetPosition() + diff);
}

void Transform::SetRotation(Transform& transform, Quaternion newVal) {
    const TransformDecomposition decomposition = transform.GetDecomposition();
    transform.matrix = FromTRS(decomposition.translation, newVal, decomposition.scale);
}

void Transform::ChangeRotation(Transform& transform, Quaternion diff) {
    SetRotation(transform, transform.GetRotation() * diff);
}

void Transform::SetScale(Transform& transform, Vector3 newVal) {
    const TransformDecomposition decomposition = transform.GetDecomposition();
    const Vector3 currentScale = decomposition.scale;
    const float requested[3] = {newVal.x, newVal.y, newVal.z};
    const float current[3] = {currentScale.x, currentScale.y, currentScale.z};

    for (std::size_t column = 0; column < 3; ++column) {
        if (current[column] == 0.0f) {
            transform.matrix = FromTRS(decomposition.translation, decomposition.rotation, newVal);
            return;
        }

        const float ratio = requested[column] / current[column];
        for (std::size_t row = 0; row < 3; ++row) {
            transform.matrix(row, column) *= ratio;
        }
    }
}

void Transform::ChangeScale(Transform& transform, Vector3 diff) {
    SetScale(transform, transform.GetScale() + diff);
}

Vector3 Transform::GetPosition() const {
    return GetDecomposition().translation;
}

Quaternion Transform::GetRotation() const {
    return GetDecomposition().rotation;
}

Vector3 Transform::GetScale() const {
    return GetDecomposition().scale;
}

TransformDecomposition Transform::GetDecomposition() const {
    return TransformUtils::Decompose(matrix);
}

Matrix4 Transform::FromTRS(
    const Vector3& position,
    const Quaternion& rotation,
    const Vector3& scale
) {
    return TransformUtils::FromTRS(position, rotation, scale);
}

/**
 * @brief Produce the world transform into result based on the parent and child transforms.
 *
 * @param result
 * @param parent
 * @param child
 */
Matrix4 Transform::ComposeTransforms(const Matrix4 &parentTransform, const Matrix4 &childTransform) {
    return parentTransform * childTransform;
}
