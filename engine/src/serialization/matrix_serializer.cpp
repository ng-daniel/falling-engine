#include "engine/serialization/matrix_serializer.h"

#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>

void MatrixSerializer::Serialize(JsonArchive& archive, const glm::mat4& matrix, std::string_view name) {
    archive.BeginArray(name);
    nlohmann::json& values = archive.GetArray();
    for (std::size_t i = 0; i < 16; ++i) {
        values.push_back(glm::value_ptr(matrix)[i]);
    }
    archive.EndArray();
}

void MatrixSerializer::Deserialize(JsonArchive& archive, glm::mat4& matrix, std::string_view name) {
    archive.BeginArray(name);
    const nlohmann::json& values = archive.GetArray();
    if (!values.is_array() || values.size() != 16) {
        archive.EndArray();
        throw std::runtime_error("Matrix JSON value must contain exactly 16 elements");
    }
    for (std::size_t i = 0; i < 16; ++i) {
        glm::value_ptr(matrix)[i] = values.at(i).get<float>();
    }
    archive.EndArray();
}
