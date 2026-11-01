#pragma once

#include <string_view>

#include "engine/serialization/jsonarchive.h"
#include <glm/mat4x4.hpp>

class MatrixSerializer {
public:
    static void Serialize(JsonArchive& archive, const glm::mat4& matrix, std::string_view name = "matrix");
    static void Deserialize(JsonArchive& archive, glm::mat4& matrix, std::string_view name = "matrix");
};
