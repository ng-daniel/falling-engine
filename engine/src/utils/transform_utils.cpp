#include "engine/utils/transform_utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

TransformDecomposition TransformUtils::Decompose(const glm::mat4& matrix) {
    glm::mat3 rotationScaleMatrix(matrix);

    TransformDecomposition decomposition {
        glm::vec3(matrix[3]),
        glm::quat::wxyz(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(
            glm::length(rotationScaleMatrix[0]),
            glm::length(rotationScaleMatrix[1]),
            glm::length(rotationScaleMatrix[2])
        )
    };

    rotationScaleMatrix[0] = glm::normalize(rotationScaleMatrix[0]);
    rotationScaleMatrix[1] = glm::normalize(rotationScaleMatrix[1]);
    rotationScaleMatrix[2] = glm::normalize(rotationScaleMatrix[2]);

    if (glm::determinant(rotationScaleMatrix) < 0.0f) {
        rotationScaleMatrix[0] = -rotationScaleMatrix[0];
        rotationScaleMatrix[1] = -rotationScaleMatrix[1];
        rotationScaleMatrix[2] = -rotationScaleMatrix[2];
    }

    decomposition.rotation = glm::quat_cast(rotationScaleMatrix);
    return decomposition;
}

glm::mat4 TransformUtils::FromTRS(
    const glm::vec3& position,
    const glm::quat& rotation,
    const glm::vec3& scale
) {
    glm::mat4 matrix(1.0f);
    matrix = glm::translate(matrix, position);
    matrix *= glm::mat4_cast(rotation);
    matrix = glm::scale(matrix, scale);
    return matrix;
}
