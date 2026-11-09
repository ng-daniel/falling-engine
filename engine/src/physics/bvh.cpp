#include "engine/physics/bvh.h"
#include "engine/debug/logger.h"
#include <cstdint>
#include <glm/common.hpp>
#include <queue>
#include <stack>
#include <vector>

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

float GetSurfaceArea(const AABB& bounds) {
    glm::vec3 dim = bounds.max - bounds.min; // width, height, depth
    return 2.0f * (dim.x * dim.y + dim.y * dim.z + dim.z * dim.x);
}

AABB Union(const AABB& a, const AABB& b) {
    return AABB{
        glm::min(a.min, b.min),
        glm::max(a.max, b.max)
    };
}

float GetCombinedSurfaceArea(const AABB& a, const AABB& b) {
    return GetSurfaceArea(Union(a, b));
}

/**
 * @brief Compute exact surface area cost for inserting new AABB into the BVH at the given node
 * 
 * @param request
 * @param node 
 * @param inheritedCost
 * @return float 
 */
float GetCostNode(const AABB& request, const AABB& node, float inheritedCost) {
    return GetCombinedSurfaceArea(request, node) + inheritedCost;
}

/**
 * @brief Computes lower bound for the cost within the subtree
 * rooted at node.
 *
 * Any cost in the subtree at node cannot be lower than
 * if there was an existing AABB leaf exactly the dimensions 
 * of our request. 
 * 
 * @param request
 * @param inheritedCost 
 * @return float 
 */
float GetCostLower(const AABB& request, const AABB& node, float inheritedCost) {
    return GetSurfaceArea(request) + GetSurfaceArea(node) + inheritedCost;
}

struct InsQueueEntry {
    BVHIndex index;
    float inheritedCost;
    float cost;
};
struct CompareQueueEntries {
    bool operator()(const InsQueueEntry& q1, const InsQueueEntry& q2) {
        // defines a min heap, return true if q1 should bubble down
        return q1.cost > q2.cost;
    }
};

/**
 * @brief 3 steps:
 * 1. Find the best option for a sibling to insert into.
 * 2. Modify the tree to insert the sibling.
 * 3. Refit Ancestor AABBs.
 * 
 * @param id 
 * @param request 
 */
void BVH::Insert(ECS_RID id, const AABB& request) {
    // empty tree case
    if (rootIndex == INVALID_BVH_INDEX) {
        rootIndex = AllocateNode(request);
        return;
    }
    
    /// PHASE 1: FIND BEST INSERTION SPOT
    /// ------------------------------------------------------

    float bestCost = std::numeric_limits<float>::max();
    BVHIndex bestNode = INVALID_BVH_INDEX;
    
    std::stack<float> stack; // tracks ancestor surface area cost
    std::priority_queue<InsQueueEntry, std::vector<InsQueueEntry>, CompareQueueEntries> queue;

    InsQueueEntry rootEntry = {
        rootIndex,
        0.0f,
        GetCostNode(request, nodes[rootIndex].bounds, 0.0f)
    };
    queue.push(rootEntry);

    while (!queue.empty()) {
        InsQueueEntry curr = queue.top();
        if (curr.cost < bestCost) {
            bestCost = curr.cost;
            bestNode = curr.index;
        } 
        
        float subtreeLowerBound = GetCostLower(
            request,
            nodes[curr.index].bounds,
            curr.inheritedCost
        );
        if (subtreeLowerBound > bestCost) {
            queue.pop();
            continue;
        }
        
        float newInheritedCost = GetSurfaceArea(nodes[curr.index].bounds) + curr.inheritedCost;
        
        BVHIndex leftIndex = nodes[curr.index].leftChild;
        InsQueueEntry leftChild = {
            leftIndex,
            newInheritedCost,
            GetCostNode(request, nodes[leftIndex].bounds, newInheritedCost)
        };
        
        BVHIndex rightIndex = nodes[curr.index].rightChild;
        InsQueueEntry rightChild = {
            rightIndex,
            newInheritedCost,
            GetCostNode(request, nodes[rightIndex].bounds, newInheritedCost)
        };

        queue.push(leftChild);
        queue.push(rightChild);

        queue.pop();
    }

    /// PHASE 2: MODIFY TREE TO ADD NODE
    /// -----------------------------------------

    AABB parentBounds = Union(request, nodes[bestNode].bounds);
    
    BVHIndex newParentIndex = AllocateNode(parentBounds);
    BVHIndex requestIndex = AllocateNode(request);

    BVHNode& newParentNode = nodes[newParentIndex];
    BVHNode& siblingNode = nodes[bestNode];
    BVHNode& requestNode = nodes[requestIndex];
    BVHNode& oldParentNode = nodes[siblingNode.parent];
    
    newParentNode.parent = siblingNode.parent;
    siblingNode.parent = newParentIndex;
    requestNode.parent = newParentIndex;
    
    if (oldParentNode.leftChild == bestNode) {
        oldParentNode.leftChild = newParentIndex;
    }
    else if (oldParentNode.rightChild == bestNode) {
        oldParentNode.rightChild = newParentIndex;
    }
    else {
        Logger::Error("BVH::Insert", "Invalid BVH tree.");
    }

    /// PHASE 3: REFIT ANCESTORS
    /// -----------------------------------------

    BVHIndex curr = newParentNode.parent;
    while (curr != INVALID_BVH_INDEX) {
        nodes[curr].bounds = Union(
            nodes[nodes[curr].leftChild].bounds,
            nodes[nodes[curr].rightChild].bounds
        );
        curr = nodes[curr].parent;
    }

    // done!
    return;
}