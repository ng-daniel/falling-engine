#include "engine/serialization/quaternion_serializer.h"

void QuaternionSerializer::Serialize(JsonArchive& archive, const Quaternion& quaternion, std::string_view name) {
    archive.BeginObject(name);
    archive.Write("x", quaternion.x);
    archive.Write("y", quaternion.y);
    archive.Write("z", quaternion.z);
    archive.Write("w", quaternion.w);
    archive.EndObject();
}

void QuaternionSerializer::Deserialize(JsonArchive& archive, Quaternion& quaternion, std::string_view name) {
    archive.Read("x", quaternion.x);
    archive.Read("y", quaternion.y);
    archive.Read("z", quaternion.z);
    archive.Read("w", quaternion.w);
}
