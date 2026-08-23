#include "engine/ecs/components/transform.h"
#include "engine/serialization/jsonarchive.h"
#include "engine/serialization/vector_serializer.h"
#include "engine/serialization/uuid_serializer.h"

void Transform::Serialize(JsonArchive& archive, const Transform& transform) {
    VectorSerializer::Serialize(archive, transform.position, "position");
    VectorSerializer::Serialize(archive, transform.rotation, "rotation");
    VectorSerializer::Serialize(archive, transform.scale, "scale");
    UUIDSerializer::Serialize(archive, transform.parentEntityId, "parentEntityId");
    UUIDSerializer::Serialize(archive, transform.firstChildEntityId, "firstChildEntityId");
    UUIDSerializer::Serialize(archive, transform.nextSiblingEntityId, "nextSiblingEntityId");
    UUIDSerializer::Serialize(archive, transform.prevSiblingEntityId, "prevSiblingEntityId");
}

Transform Transform::Deserialize(JsonArchive& archive) {
    Transform transform;
    transform.position = VectorSerializer::DeserializeVector3(archive, "position");
    transform.rotation = VectorSerializer::DeserializeVector4(archive, "rotation");
    transform.scale = VectorSerializer::DeserializeVector3(archive, "scale");
    transform.parentEntityId = UUIDSerializer::Deserialize(archive, "parentEntityId");
    transform.firstChildEntityId = UUIDSerializer::Deserialize(archive, "firstChildEntityId");
    transform.nextSiblingEntityId = UUIDSerializer::Deserialize(archive, "nextSiblingEntityId");
    transform.prevSiblingEntityId = UUIDSerializer::Deserialize(archive, "prevSiblingEntityId");

    return transform;
}