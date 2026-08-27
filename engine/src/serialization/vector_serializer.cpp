#include "engine/serialization/vector_serializer.h"

void VectorSerializer::Serialize(JsonArchive& archive, const Vector2& vector, std::string_view name) {
    archive.BeginObject(name);
    archive.Write("x", vector.x);
    archive.Write("y", vector.y);
    archive.EndObject();
}

void VectorSerializer::Serialize(JsonArchive& archive, const Vector3& vector, std::string_view name) {
    archive.BeginObject(name);
    archive.Write("x", vector.x);
    archive.Write("y", vector.y);
    archive.Write("z", vector.z);
    archive.EndObject();
}

void VectorSerializer::Serialize(JsonArchive& archive, const Vector4& vector, std::string_view name) {
    archive.BeginObject(name);
    archive.Write("x", vector.x);
    archive.Write("y", vector.y);
    archive.Write("z", vector.z);
    archive.Write("w", vector.w);
    archive.EndObject();
}

void VectorSerializer::DeserializeVector2(JsonArchive& archive, Vector2& vector, std::string_view name) {
    archive.Read("x", vector.x);
    archive.Read("y", vector.y);
}

void VectorSerializer::DeserializeVector3(JsonArchive& archive, Vector3& vector, std::string_view name) {
    archive.Read("x", vector.x);
    archive.Read("y", vector.y);
    archive.Read("z", vector.z);
}

void VectorSerializer::DeserializeVector4(JsonArchive& archive, Vector4& vector, std::string_view name) {
    archive.Read("x", vector.x);
    archive.Read("y", vector.y);
    archive.Read("z", vector.z);
    archive.Read("w", vector.w);
}