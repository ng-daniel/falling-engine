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

void Transform::Deserialize(JsonArchive& archive, Transform& transform) {
    VectorSerializer::DeserializeVector3(archive, transform.position, "position");
    VectorSerializer::DeserializeVector4(archive, transform.rotation, "rotation");
    VectorSerializer::DeserializeVector3(archive, transform.scale, "scale");
    UUIDSerializer::Deserialize(archive, transform.parentEntityId, "parentEntityId");
    UUIDSerializer::Deserialize(archive, transform.firstChildEntityId, "firstChildEntityId");
    UUIDSerializer::Deserialize(archive, transform.nextSiblingEntityId, "nextSiblingEntityId");
    UUIDSerializer::Deserialize(archive, transform.prevSiblingEntityId, "prevSiblingEntityId");
}