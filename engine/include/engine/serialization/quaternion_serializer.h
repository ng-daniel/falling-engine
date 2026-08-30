#pragma once

#include "engine/serialization/jsonarchive.h"
#include "engine/utils/quaternion.h"

class QuaternionSerializer {
public:
    static void Serialize(JsonArchive& archive, const Quaternion& quaternion, std::string_view name = "");
    static void Deserialize(JsonArchive& archive, Quaternion& quaternion, std::string_view name = "");
};
