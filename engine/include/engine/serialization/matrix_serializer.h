#pragma once

#include <string_view>

#include "engine/serialization/jsonarchive.h"
#include "engine/utils/matrix.h"

class MatrixSerializer {
public:
    static void Serialize(JsonArchive& archive, const Matrix4& matrix, std::string_view name = "matrix");
    static void Deserialize(JsonArchive& archive, Matrix4& matrix, std::string_view name = "matrix");
};
