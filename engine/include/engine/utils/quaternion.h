#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

struct Quaternion {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

    // constructors
	constexpr Quaternion() = default;
	constexpr explicit Quaternion(float scalar)
        : x(scalar), y(scalar), z(scalar), w(scalar) {}
	constexpr Quaternion(float xValue, float yValue, float zValue, float wValue)
		: x(xValue), y(yValue), z(zValue), w(wValue) {}
	constexpr Quaternion(const glm::quat& value)
	    : x(value.x), y(value.y), z(value.z), w(value.w) {}
	constexpr Quaternion& operator=(const glm::quat& value) {
		x = value.x;
		y = value.y;
		z = value.z;
		w = value.w;
		return *this;
	}

    // hamilton product (quaternion multiplication)
	constexpr Quaternion operator*(const Quaternion& other) const {
		return Quaternion(
			w * other.x + x * other.w + y * other.z - z * other.y,
			w * other.y - x * other.z + y * other.w + z * other.x,
			w * other.z + x * other.y - y * other.x + z * other.w,
			w * other.w - x * other.x - y * other.y - z * other.z
		);
	}
	static Quaternion MatrixToQuaternion(const float matrix[16]);
};
