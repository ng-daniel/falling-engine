#ifndef ENGINE_ECS_COMPONENTS_TRANSFORM_H
#define ENGINE_ECS_COMPONENTS_TRANSFORM_H

#include "engine/utils/uuid.h"
#include "engine/ecs/ecs_structures.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief The transform component
 * Values here are local
 * To get the world value of something, 
 * you need to apply the parent's transform.
 * 
 */
struct Transform : public Component {
    glm::vec3 position;
    glm::vec4 rotation;
    glm::vec3 scale;

    // hierarchy references
    UUID parentEntityId;
    UUID firstChildEntityId;
    UUID nextSiblingEntityId;
    UUID prevSiblingEntityId;
};

/**
 * @brief Utility class used to perform operations on transform components
 * 
 */
class TransformOps {
public:
    TransformOps() = delete;

    static void SetPosition(Transform& transform, glm::vec3 newVal);
    static void ChangeRotation(Transform& transform, glm::vec3 diff);

    static void SetRotation(Transform& transform, glm::vec4 newVal);
    static void ChangeRotation(Transform& transform, glm::vec4 diff);

    static void SetScale(Transform& transform, glm::vec3 newVal);
    static void ChangeScale(Transform& transform, glm::vec3 diff);
};

#endif // ENGINE_ECS_COMPONENTS_TRANSFORM_H