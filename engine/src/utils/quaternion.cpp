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
