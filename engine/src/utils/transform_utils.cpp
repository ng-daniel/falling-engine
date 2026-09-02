#include "engine/utils/transform_utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

TransformDecomposition TransformUtils::Decompose(const Matrix4& matrix) {
    glm::mat3 rotationScaleMatrix = {
        matrix(0, 0), matrix(1, 0), matrix(2, 0),
        matrix(0, 1), matrix(1, 1), matrix(2, 1),
        matrix(0, 2), matrix(1, 2), matrix(2, 2)
    };

    TransformDecomposition decomposition {
        Vector3(matrix(0, 3), matrix(1, 3), matrix(2, 3)),
        Quaternion(),
        Vector3(
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

    decomposition.rotation = Quaternion(glm::quat_cast(rotationScaleMatrix));
    return decomposition;
}

Matrix4 TransformUtils::FromTRS(
    const Vector3& position,
    const Quaternion& rotation,
    const Vector3& scale
) {
    glm::mat4 matrix(1.0f);
    matrix = glm::translate(matrix, glm::vec3(position.x, position.y, position.z));
    matrix *= glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
    matrix = glm::scale(matrix, glm::vec3(scale.x, scale.y, scale.z));
    return Matrix4(matrix);
}
