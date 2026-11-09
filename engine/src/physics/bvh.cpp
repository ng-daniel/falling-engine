#include "engine/physics/bvh.h"
#include "engine/debug/logger.h"
#include <cstdint>
#include <glm/common.hpp>
#include <queue>
#include <stack>
#include <vector>

BVH::BVH() {
    InitializeFreeList();
}

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
    
    nodeCount++;

    return nodeIndex;
}

/**
 * @brief Frees a node and adds it back to the front of the implicit free list
 * Add to the front so we don't just keep shifting the head backwards
 * this keeps most free nodes towards the low-index part of the array,
 * even if it starts to get fragmented
 *
 * also increments generation version so we can identify outdated
 * BVHNode handle references
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
    node.generation++;

    nodeCount--;
    
    return nodeIndex;
}


/// SURFACE AREA OPS
/// ---------------------------

AABB Union(const AABB& a, const AABB& b) {
    return AABB{
        glm::min(a.min, b.min),
        glm::max(a.max, b.max)
    };
}

float SA(const AABB& bounds) {
    glm::vec3 dim = bounds.max - bounds.min; // width, height, depth
    return 2.0f * (dim.x * dim.y + dim.y * dim.z + dim.z * dim.x);
}

/// COST FUNCTIONS
/// --------------------------

/**
 * @brief Computes inhereted cost
 * 
 * @param request The AABB we want to insert
 * @param node The node we want to add to the accumulated inherited cost
 * @param prevInherited The previously computed inherited cost, which
 * consists of every ancestor above node
 * @return float 
 */
float InheritedCost(const AABB& request, const AABB& node, float prevInherited) {
    return SA(Union(node, request)) - SA(node) + prevInherited;
}

/**
 * @brief Compute exact surface area cost for inserting new AABB into the BVH at the given node
 * 
 * @param request
 * @param node 
 * @param inheritedCost
 * @return float 
 */
float DirectCost(const AABB& request, const AABB& node) {
    return SA(Union(request, node));
}

/**
 * @brief Cost = Cost of creating a new internal node and
 * updating all the nodes, starting from current node's parent
 * 
 * @param request 
 * @param node 
 * @param prevInherited the inherited cost of node's ancestors,
 * since insertting at this point makes "node" a child of the new internal node
 * @return float 
 */
float Cost(const AABB& request, const AABB& node, const float prevInherited) {
    return DirectCost(request, node) + prevInherited;
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
float GetCostLower(const AABB& request, float inheritedCost) {
    return SA(request) + inheritedCost;
}

/// INSERTION PRIORITY QUEUE STRUCTS
/// ----------------------------------------

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
    if (handles.contains(id)) {
        Logger::Error("BVH::Insert", "ECS runtime ID already exists in the tree/handle mapping.");
        return;
    }
    
    // empty tree case
    if (rootIndex == INVALID_BVH_INDEX) {
        rootIndex = AllocateNode(request);
        if (rootIndex == INVALID_BVH_INDEX) {
            Logger::Error("BVH::Insert", "Allocate node failed.");
            return;
        }
        handles.emplace(id, BVHNodeHandle{
            rootIndex,
            nodes[rootIndex].generation
        });
        nodes[rootIndex].entityId = id;    
        return;
    }
    
    /// PHASE 1: FIND BEST INSERTION SPOT
    /// ------------------------------------------------------

    float bestCost = std::numeric_limits<float>::max();
    BVHIndex bestIndex = INVALID_BVH_INDEX;
    
    std::priority_queue<InsQueueEntry, std::vector<InsQueueEntry>, CompareQueueEntries> queue;

    InsQueueEntry rootEntry = {
        rootIndex,
        0.0f,
        Cost(request, nodes[rootIndex].bounds, 0.0f)
    };
    queue.push(rootEntry);

    while (!queue.empty()) {
        InsQueueEntry curr = queue.top();
        queue.pop();

        if (curr.cost < bestCost) {
            bestCost = curr.cost;
            bestIndex = curr.index;
        } 
        
        // determine if it's worth it to explore this node's subtree

        AABB& currentBounds = nodes[curr.index].bounds;
        float newInheritedCost = InheritedCost(
            request,
            currentBounds,
            curr.inheritedCost
        );
        float subtreeLowerBound = GetCostLower(
            request,
            newInheritedCost
        );
        if (!IsLeaf(curr.index) && subtreeLowerBound < bestCost) {    
            
            // enqueue children if worthy

            BVHIndex leftIndex = nodes[curr.index].leftChild;
            InsQueueEntry leftChild = {
                leftIndex,
                newInheritedCost,
                Cost(request, nodes[leftIndex].bounds, newInheritedCost)
            };
            
            BVHIndex rightIndex = nodes[curr.index].rightChild;
            InsQueueEntry rightChild = {
                rightIndex,
                newInheritedCost,
                Cost(request, nodes[rightIndex].bounds, newInheritedCost)
            };

            queue.push(leftChild);
            queue.push(rightChild);
        }
    }

    /// PHASE 2: MODIFY TREE TO ADD NODE
    /// -----------------------------------------

    AABB parentBounds = Union(request, nodes[bestIndex].bounds);
    
    BVHIndex newParentIndex = AllocateNode(parentBounds);
    if (newParentIndex == INVALID_BVH_INDEX) {
        Logger::Error("BVH::Insert", "Allocation failed.");
        return;
    }
    BVHIndex requestIndex = AllocateNode(request);
    if (requestIndex == INVALID_BVH_INDEX) {
        Logger::Error("BVH::Insert", "Allocation failed.");
        FreeNode(newParentIndex);
        return;
    }

    BVHNode& newParentNode = nodes[newParentIndex];
    BVHNode& siblingNode = nodes[bestIndex];
    BVHNode& requestNode = nodes[requestIndex];

    BVHIndex oldParentIndex = siblingNode.parent;
    
    newParentNode.parent = siblingNode.parent;
    newParentNode.leftChild = bestIndex;
    newParentNode.rightChild = requestIndex;
    siblingNode.parent = newParentIndex;
    requestNode.parent = newParentIndex;
    
    if (oldParentIndex == INVALID_BVH_INDEX) {
        rootIndex = newParentIndex;
    }
    else {
        BVHNode& oldParentNode = nodes[oldParentIndex];
        if (oldParentNode.leftChild == bestIndex) {
            oldParentNode.leftChild = newParentIndex;
        }
        else if (oldParentNode.rightChild == bestIndex) {
            oldParentNode.rightChild = newParentIndex;
        }
        else {
            Logger::Error("BVH::Insert", "Invalid BVH tree.");
        }
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

    // mini phase 4: ecs mapping
    
    handles.emplace(id, BVHNodeHandle{
        requestIndex,
        requestNode.generation
    });
    nodes[requestIndex].entityId = id;    

    // done!
    return;
}