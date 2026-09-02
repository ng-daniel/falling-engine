#pragma once

#include <array>
#include <cstddef>

#include <glm/mat4x4.hpp>
class Matrix4 {
public:
    Matrix4();
    explicit Matrix4(float diagonal);
    explicit Matrix4(const glm::mat4& matrix);
    explicit Matrix4(const float* data); // for 16 element 1d arrays

    float& operator()(std::size_t row, std::size_t column);
    const float& operator()(std::size_t row, std::size_t column) const;
    Matrix4 operator*(const Matrix4& other) const;

    glm::mat4 ToGlm() const;
    const float* Data() const;
    float* Data();

private:
    // column-major storage
    std::array<float, 16> elements;
};
