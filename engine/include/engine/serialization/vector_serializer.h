#pragma once

#include "engine/serialization/jsonarchive.h"
#include "engine/utils/vector.h"

class VectorSerializer {
public:
    static void Serialize(JsonArchive& archive, const Vector2& vector, std::string_view name = "");
    static void Serialize(JsonArchive& archive, const Vector3& vector, std::string_view name = "");
    static void Serialize(JsonArchive& archive, const Vector4& vector, std::string_view name = "");

    static Vector2 DeserializeVector2(JsonArchive& archive, std::string_view name = "");
    static Vector3 DeserializeVector3(JsonArchive& archive, std::string_view name = "");
    static Vector4 DeserializeVector4(JsonArchive& archive, std::string_view name = "");
};