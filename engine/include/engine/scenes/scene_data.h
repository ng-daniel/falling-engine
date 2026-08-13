#ifndef ENGINE_SCENES_SCENE_DATA_H
#define ENGINE_SCENES_SCENE_DATA_H

#include <engine/utils/uuid.h>

#include <vector>

/**
 * @brief Represents a node in a scene
 * Could be the root of a scene too
 * It doesn't matter
 */
class Node {
    UUID entityId;
    std::vector<UUID> children;    
};

#endif // ENGINE_SCENES_SCENE_DATA_H