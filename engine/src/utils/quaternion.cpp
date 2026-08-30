#include "engine/utils/quaternion.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// note that input matrix is column-major order transformation matrix
Quaternion Quaternion::MatrixToQuaternion(const float matrix[16]) {
    // take upper left 3x3 as rotation + scale
    
    glm::mat3 rotationScaleMatrix = {
        matrix[0], matrix[1], matrix[2],
        matrix[4], matrix[5], matrix[6],
        matrix[8], matrix[9], matrix[10]
    };
    
    // normalize the rotationScale matrix to remove scaling
    
    rotationScaleMatrix[0] = glm::normalize(rotationScaleMatrix[0]);
    rotationScaleMatrix[1] = glm::normalize(rotationScaleMatrix[1]);
    rotationScaleMatrix[2] = glm::normalize(rotationScaleMatrix[2]);

    // convert to quaternion, return as engine specific format
    
    glm::quat quat = glm::quat_cast(rotationScaleMatrix);
    return Quaternion(quat.x, quat.y, quat.z, quat.w);
}