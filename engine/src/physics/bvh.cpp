#include "engine/physics/bvh.h"
#include <cstdint>
#include <glm/common.hpp>

bool BVH::IsLeaf(BVHIndex nodeIndex) {
    BVHNode& node = nodes[nodeIndex];
    return node.leftChild == INVALID_BVH_INDEX && 
        node.rightChild == INVALID_BVH_INDEX;
}

/**
 * @brief Hooks up parent child pointers in the implicit free list
 */
void BVH::InitializeFreeList() {
    for (uint32_t i = 0; i < MAX_NODES; ++i) {
        nodes[i].parent = i>0 ? i-1 : INVALID_BVH_INDEX;
        nodes[i].leftChild = INVALID_BVH_INDEX;
        nodes[i].rightChild = i<MAX_NODES-1 ? i+1 : INVALID_BVH_INDEX;
    }
    freeListHead = 0;
}

/**
 * @brief Allocates a node from the implicit free list
 * Does NOT do any insertion stuff, that is handled separately in Insert()
 * 
 * @param bounds 
 * @return BVHIndex 
 */
BVHIndex BVH::AllocateNode(const AABB& bounds) {
    if (freeListHead == INVALID_BVH_INDEX) {
        return INVALID_BVH_INDEX; // no free nodes available
    }
    BVHIndex nodeIndex = freeListHead;
    BVHNode& node = nodes[nodeIndex];

    freeListHead = node.rightChild; // move head to next free node

    node.bounds = bounds;
    node.parent = INVALID_BVH_INDEX;
    node.leftChild = INVALID_BVH_INDEX;
    node.rightChild = INVALID_BVH_INDEX;
    node.isFree = false;
    
    return nodeIndex;
}

/**
 * @brief Frees a node and adds it back to the front of the implicit free list
 * Add to the front so we don't just keep shifting the head backwards
 * this keeps most free nodes towards the low-index part of the array,
 * even if it starts to get fragmented
 * 
 * @param nodeIndex 
 * @return BVHIndex 
 */
BVHIndex BVH::FreeNode(BVHIndex nodeIndex) {
    if (nodeIndex == INVALID_BVH_INDEX) {
        return INVALID_BVH_INDEX;
    }
    BVHNode& node = nodes[nodeIndex];
    node.parent = INVALID_BVH_INDEX;
    node.leftChild = INVALID_BVH_INDEX;
    node.rightChild = freeListHead;
    node.isFree = true;
    freeListHead = nodeIndex;
    return nodeIndex;
}

float BVH::GetSurfaceArea(const AABB& bounds) {
    glm::vec3 dim = bounds.max - bounds.min; // width, height, depth
    return 2.0f * (dim.x * dim.y + dim.y * dim.z + dim.z * dim.x);
}

float BVH::GetCombinedSurfaceArea(const AABB& a, const AABB& b) {
    AABB combined;
    combined.min = glm::min(a.min, b.min);
    combined.max = glm::max(a.max, b.max);
    return GetSurfaceArea(combined);
}

void BVH::Insert(ECS_RID id, const AABB& bounds) {
    // empty tree case
    if (rootIndex == INVALID_BVH_INDEX) {
        rootIndex = AllocateNode(bounds);
        return;
    }
    
    // non-empty tree case
    float cost = 0;
    BVHIndex current = rootIndex;
    while (true) {
        float currentSA = GetSurfaceArea(nodes[current].bounds);
        float currentCombinedSA = GetCombinedSurfaceArea(nodes[current].bounds, bounds);
        
        // estimated inherited cost, not exact, just a heuristic
        // inserting at current node may also cause ancestor AABBs to increase as well
        // this is represented by an approximate coefficient of 2
        float inheritedCost = 2 * (currentCombinedSA - currentSA);
    }   
}