#include "engine/utils/matrix.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
    glm::quat QuaternionToGlm(const Quaternion& quaternion) {
        return glm::quat(quaternion.w, quaternion.x, quaternion.y, quaternion.z);
    }
}

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

/// GET TRS OPERATIONS
/// -----------------------------------

Vector3 Matrix4::GetPosition() const {
    return Vector3((*this)(0, 3), (*this)(1, 3), (*this)(2, 3));
}

Vector3 Matrix4::GetScale() const {
    return Vector3::MatrixToScale(elements.data());
}

Quaternion Matrix4::GetRotation() const {
    return Quaternion::MatrixToQuaternion(elements.data());
}

/// SET TRS OPERATIONS
/// -----------------------------------

void Matrix4::SetPosition(const Vector3& position) {
    (*this)(0, 3) = position.x;
    (*this)(1, 3) = position.y;
    (*this)(2, 3) = position.z;
}

void Matrix4::SetRotation(const Quaternion& rotation) {
    *this = FromTRS(GetPosition(), rotation, GetScale());
}

void Matrix4::SetScale(const Vector3& scale) {
    const Vector3 currentScale = GetScale();
    const float requested[3] = {scale.x, scale.y, scale.z};
    const float current[3] = {currentScale.x, currentScale.y, currentScale.z};

    for (std::size_t column = 0; column < 3; ++column) {
        if (current[column] == 0.0f) {
            // A collapsed axis has no direction to preserve. Rebuild a valid TRS matrix.
            *this = FromTRS(GetPosition(), GetRotation(), scale);
            return;
        }
        const float ratio = requested[column] / current[column];
        for (std::size_t row = 0; row < 3; ++row) {
            (*this)(row, column) *= ratio;
        }
    }
}

/**
 * @brief Construct a transformation matrix from translation, rotation, and scale
 * 
 * @param position 
 * @param rotation 
 * @param scale 
 * @return Matrix4 
 */
Matrix4 Matrix4::FromTRS(
    const Vector3& position,
    const Quaternion& rotation,
    const Vector3& scale
) {
    glm::mat4 matrix(1.0f);
    matrix = glm::translate(matrix, glm::vec3(position.x, position.y, position.z));
    matrix *= glm::mat4_cast(QuaternionToGlm(rotation));
    matrix = glm::scale(matrix, glm::vec3(scale.x, scale.y, scale.z));
    return Matrix4(matrix);
}
