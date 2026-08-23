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

Vector2 VectorSerializer::DeserializeVector2(JsonArchive& archive, std::string_view name) {
    Vector2 vector;
    archive.Read("x", vector.x);
    archive.Read("y", vector.y);
    return vector;
}

Vector3 VectorSerializer::DeserializeVector3(JsonArchive& archive, std::string_view name) {
    Vector3 vector;
    archive.Read("x", vector.x);
    archive.Read("y", vector.y);
    archive.Read("z", vector.z);
    return vector;
}

Vector4 VectorSerializer::DeserializeVector4(JsonArchive& archive, std::string_view name) {
    Vector4 vector;
    archive.Read("x", vector.x);
    archive.Read("y", vector.y);
    archive.Read("z", vector.z);
    archive.Read("w", vector.w);
    return vector;
}