#include "engine/serialization/matrix_serializer.h"

#include <stdexcept>

void MatrixSerializer::Serialize(JsonArchive& archive, const Matrix4& matrix, std::string_view name) {
    archive.BeginArray(name);
    nlohmann::json& values = archive.GetArray();
    for (std::size_t i = 0; i < 16; ++i) {
        values.push_back(matrix.Data()[i]);
    }
    archive.EndArray();
}

void MatrixSerializer::Deserialize(JsonArchive& archive, Matrix4& matrix, std::string_view name) {
    archive.BeginArray(name);
    const nlohmann::json& values = archive.GetArray();
    if (!values.is_array() || values.size() != 16) {
        archive.EndArray();
        throw std::runtime_error("Matrix4 JSON value must contain exactly 16 elements");
    }
    for (std::size_t i = 0; i < 16; ++i) {
        matrix.Data()[i] = values.at(i).get<float>();
    }
    archive.EndArray();
}
