#pragma once

#include "engine/serialization/jsonarchive.h"
#include "engine/utils/vector.h"

class VectorSerializer {
public:
    static void Serialize(JsonArchive& archive, const Vector2& vector, std::string_view name = "");
    static void Serialize(JsonArchive& archive, const Vector3& vector, std::string_view name = "");
    static void Serialize(JsonArchive& archive, const Vector4& vector, std::string_view name = "");

    static void DeserializeVector2(JsonArchive& archive, Vector2& vector, std::string_view name = "");
    static void DeserializeVector3(JsonArchive& archive, Vector3& vector, std::string_view name = "");
    static void DeserializeVector4(JsonArchive& archive, Vector4& vector, std::string_view name = "");
};