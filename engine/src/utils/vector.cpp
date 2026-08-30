#include "engine/utils/vector.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Vector3 Vector3::MatrixToScale(const float matrix[16]) {
    // take upper left 3x3 as rotation + scale
    
    glm::mat3 rotationScaleMatrix = {
        matrix[0], matrix[1], matrix[2],
        matrix[4], matrix[5], matrix[6],
        matrix[8], matrix[9], matrix[10]
    };

    // extract the scale factors from the rotationScaleMatrix by computing the length of each column vector
    float scaleX = glm::length(rotationScaleMatrix[0]);
    float scaleY = glm::length(rotationScaleMatrix[1]);
    float scaleZ = glm::length(rotationScaleMatrix[2]);

    return Vector3(scaleX, scaleY, scaleZ);
}

Vector3 Vector3::MatrixToTranslation(const float matrix[16]) {
    // extract the first 3 rows of the 4th column (translation component)
    return Vector3(matrix[12], matrix[13], matrix[14]);
}