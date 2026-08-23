#pragma once

#include <glm/glm.hpp>

struct Vector2 {
	float x = 0.0f;
	float y = 0.0f;

    // constructors
	constexpr Vector2() = default;
	constexpr explicit Vector2(float scalar)
        : x(scalar), y(scalar) {}
	constexpr Vector2(float xValue, float yValue)
		: x(xValue), y(yValue) {}
	constexpr Vector2(const glm::vec2& value) : x(value.x), y(value.y) {}
	constexpr Vector2& operator=(const glm::vec2& value) {
		x = value.x;
		y = value.y;
		return *this;
	}
};

struct Vector3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

    // constructors
	constexpr Vector3() = default;
	constexpr explicit Vector3(float scalar) 
        : x(scalar), y(scalar), z(scalar) {}
	constexpr Vector3(float xValue, float yValue, float zValue)
		: x(xValue), y(yValue), z(zValue) {}
	constexpr Vector3(const glm::vec3& value) : x(value.x), y(value.y), z(value.z) {}
	constexpr Vector3& operator=(const glm::vec3& value) {
		x = value.x;
		y = value.y;
		z = value.z;
		return *this;
	}
};

struct Vector4 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

    // constructors
	constexpr Vector4() = default;
	constexpr explicit Vector4(float scalar)
        : x(scalar), y(scalar), z(scalar), w(scalar) {}
	constexpr Vector4(float xValue, float yValue, float zValue, float wValue)
		: x(xValue), y(yValue), z(zValue), w(wValue) {}
	constexpr Vector4(const glm::vec4& value)
	    : x(value.x), y(value.y), z(value.z), w(value.w) {}
	constexpr Vector4& operator=(const glm::vec4& value) {
		x = value.x;
		y = value.y;
		z = value.z;
		w = value.w;
		return *this;
	}
};
