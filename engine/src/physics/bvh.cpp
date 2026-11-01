#include "engine/physics/bvh.h"

bool BVH::IsLeaf(BVHIndex nodeIndex) {
    BVHNode& node = nodes[nodeIndex];
    return node.leftChild == INVALID_BVH_INDEX && 
        node.rightChild == INVALID_BVH_INDEX;
}

void BVH::InitializeFreeList() {
    for (uint32_t i = 0; i < MAX_NODES; ++i) {
        nodes[i].parent = i>0 ? i-1 : INVALID_BVH_INDEX;
        nodes[i].leftChild = INVALID_BVH_INDEX;
        nodes[i].rightChild = i<MAX_NODES-1 ? i+1 : INVALID_BVH_INDEX;
    }
    freeListHead = 0;
}

BVHIndex BVH::AllocateNode() {
    if (freeListHead == INVALID_BVH_INDEX) {
        return INVALID_BVH_INDEX; // no free nodes available
    }
    BVHIndex nodeIndex = freeListHead;
    BVHNode& node = nodes[nodeIndex];
    
    freeListHead = node.rightChild; // move head to next free node
    node.parent = INVALID_BVH_INDEX;
    node.leftChild = INVALID_BVH_INDEX;
    node.rightChild = INVALID_BVH_INDEX;
    node.isFree = false;
    
    return nodeIndex;
}

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

float BVH::GetSurfaceArea(AABB& bounds) {
    Vector3 dim = bounds.max - bounds.min; // width, height, depth
    return 2.0f * (dim.x * dim.y + dim.y * dim.z + dim.z * dim.x);
}

void BVH::Insert(ECS_RID id, const AABB& bounds) {
    if (rootIndex == INVALID_BVH_INDEX) {
        rootIndex = 0;
    }
    
    float cost = 0;
    while (true) {

    }   
}