#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

struct TransformDecomposition {
    glm::vec3 translation = glm::vec3(0.0f);
    glm::quat rotation = glm::quat::wxyz(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(0.0f);
};

class TransformUtils {
public:
    static TransformDecomposition Decompose(const glm::mat4& matrix);
    static glm::mat4 FromTRS(
        const glm::vec3& position,
        const glm::quat& rotation,
        const glm::vec3& scale
    );
};
