#include "engine/utils/quaternion.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Quaternion Quaternion::EulerToQuaternion(float pitch, float yaw, float roll) {
    glm::quat quat = glm::quat(
        glm::vec3(
            glm::radians(pitch),
            glm::radians(yaw),
            glm::radians(roll)
        )
    );
    return Quaternion(quat.x, quat.y, quat.z, quat.w);
}

/**
 * @brief Creates a quaternion representing a rotation of a specified angle around a given axis.
 * 
 * @param angle The rotation angle in degrees.
 * @param axis The axis around which to rotate.
 * @return Quaternion The resulting quaternion representing the rotation.
 */
Quaternion Quaternion::AngleAxis(float angle, const Vector3& axis) {
    glm::quat quat = glm::angleAxis(glm::radians(angle), glm::vec3(axis.x, axis.y, axis.z));
    return Quaternion(quat.x, quat.y, quat.z, quat.w);
}
