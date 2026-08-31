#include "engine/ecs/components/transform.h"
#include "engine/serialization/jsonarchive.h"
#include "engine/serialization/vector_serializer.h"
#include "engine/serialization/quaternion_serializer.h"
#include "engine/serialization/uuid_serializer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Transform::Serialize(JsonArchive& archive, const Transform& transform) {
    VectorSerializer::Serialize(archive, transform.position, "position");
    QuaternionSerializer::Serialize(archive, transform.rotation, "rotation");
    VectorSerializer::Serialize(archive, transform.scale, "scale");
    UUIDSerializer::Serialize(archive, transform.parentEntityId, "parentEntityId");
    UUIDSerializer::Serialize(archive, transform.firstChildEntityId, "firstChildEntityId");
    UUIDSerializer::Serialize(archive, transform.nextSiblingEntityId, "nextSiblingEntityId");
    UUIDSerializer::Serialize(archive, transform.prevSiblingEntityId, "prevSiblingEntityId");
}

void Transform::Deserialize(JsonArchive& archive, Transform& transform) {
    VectorSerializer::DeserializeVector3(archive, transform.position, "position");
    QuaternionSerializer::Deserialize(archive, transform.rotation, "rotation");
    VectorSerializer::DeserializeVector3(archive, transform.scale, "scale");
    UUIDSerializer::Deserialize(archive, transform.parentEntityId, "parentEntityId");
    UUIDSerializer::Deserialize(archive, transform.firstChildEntityId, "firstChildEntityId");
    UUIDSerializer::Deserialize(archive, transform.nextSiblingEntityId, "nextSiblingEntityId");
    UUIDSerializer::Deserialize(archive, transform.prevSiblingEntityId, "prevSiblingEntityId");
}

void Transform::SetPosition(Transform& transform, Vector3 newVal) {
    transform.position = newVal;
}

void Transform::ChangePosition(Transform& transform, Vector3 diff) {
    transform.position.x += diff.x;
    transform.position.y += diff.y;
    transform.position.z += diff.z;
}

void Transform::SetRotation(Transform& transform, Quaternion newVal) {
    transform.rotation = newVal;
}

void Transform::ChangeRotation(Transform& transform, Quaternion diff) {
    transform.rotation = transform.rotation * diff;
}

void Transform::SetScale(Transform& transform, Vector3 newVal) {
    transform.scale = newVal;
}

void Transform::ChangeScale(Transform& transform, Vector3 diff) {
    transform.scale.x += diff.x;
    transform.scale.y += diff.y;
    transform.scale.z += diff.z;
}