#pragma once

#include "engine/ecs/ecs_runtime_id.h"
#include <glm/vec3.hpp>

// my father: https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf

using BVHIndex = uint32_t;
constexpr BVHIndex INVALID_BVH_INDEX = BVHIndex(-1);

struct AABB {
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);
};

struct BVHNodeHandle {
    BVHIndex index;
    uint32_t generation; // invalid checking
};

struct BVHNode {
    ECS_RID entityId = INVALID_ECS_RID;
    AABB bounds;

    BVHIndex parent = INVALID_BVH_INDEX;        // parent if active, free list prev if free
    BVHIndex leftChild = INVALID_BVH_INDEX;     // left child if active, N/A if free
    BVHIndex rightChild = INVALID_BVH_INDEX;    // right child if active, free list next if free

    uint32_t generation = 0;
    bool isFree = true;
};

class BVH {
public:
    BVH();
    ~BVH() {}

    void Insert(ECS_RID id, const AABB& bounds);
    void Remove(ECS_RID id);

private:
    static constexpr uint32_t MAX_NODES = 8192;

    // unchanging, implicit data structure
    std::vector<BVHNode> nodes = std::vector<BVHNode>(MAX_NODES);
    std::unordered_map<ECS_RID, BVHNodeHandle> handles;
    
    BVHIndex rootIndex = INVALID_BVH_INDEX;
    uint32_t nodeCount = 0;
    BVHIndex freeListHead = 0; // head of the free list for unused nodes
                               // on startup, all nodes are free
    void InitializeFreeList();
    BVHIndex AllocateNode(const AABB& bounds);
    void FreeNode(BVHIndex nodeIndex);

    bool IsLeaf(BVHIndex nodeIndex);
};