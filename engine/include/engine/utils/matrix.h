#pragma once

#include <array>
#include <cstddef>

#include <glm/mat4x4.hpp>

#include "engine/utils/quaternion.h"
#include "engine/utils/vector.h"

class Matrix4 {
public:
    Matrix4();
    explicit Matrix4(float diagonal);
    explicit Matrix4(const glm::mat4& matrix);

    float& operator()(std::size_t row, std::size_t column);
    const float& operator()(std::size_t row, std::size_t column) const;
    Matrix4 operator*(const Matrix4& other) const;

    glm::mat4 ToGlm() const;
    const float* Data() const;
    float* Data();

    /// TRS OPERATIONS
    /// *assuming this matrix is being used to represent transformations
    /// --------------------------------------------------------------------

    Vector3 GetPosition() const;
    Quaternion GetRotation() const;
    Vector3 GetScale() const;
    
    void SetPosition(const Vector3& position);
    void SetRotation(const Quaternion& rotation);
    void SetScale(const Vector3& scale);
    
    static Matrix4 FromTRS(
        const Vector3& position,
        const Quaternion& rotation,
        const Vector3& scale
    );

private:
    // column-major storage
    std::array<float, 16> elements;
};
