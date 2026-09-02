#include "engine/utils/matrix.h"

#include <glm/gtc/type_ptr.hpp>

// constructors
Matrix4::Matrix4() : Matrix4(1.0f) {}
Matrix4::Matrix4(float diagonal) : elements{} {
    (*this)(0, 0) = diagonal;
    (*this)(1, 1) = diagonal;
    (*this)(2, 2) = diagonal;
    (*this)(3, 3) = diagonal;
}
Matrix4::Matrix4(const glm::mat4& matrix) {
    const float* source = glm::value_ptr(matrix);
    for (std::size_t i = 0; i < elements.size(); ++i) {
        elements[i] = source[i];
    }
}
Matrix4::Matrix4(const float* data) {
    for (std::size_t i = 0; i < elements.size(); ++i) {
        elements[i] = data[i];
    }
}

// accessor for matrix elements
float& Matrix4::operator()(std::size_t row, std::size_t column) {
    return elements[column * 4 + row];
}
const float& Matrix4::operator()(std::size_t row, std::size_t column) const {
    return elements[column * 4 + row];
}

// MATMUL
Matrix4 Matrix4::operator*(const Matrix4& other) const {
    return Matrix4(ToGlm() * other.ToGlm());
}
glm::mat4 Matrix4::ToGlm() const {
    return glm::make_mat4(elements.data());
}
const float* Matrix4::Data() const { return elements.data(); }
float* Matrix4::Data() { return elements.data(); }
