#include "engine/ecs/components/transform.h"
#include "engine/ecs/ecs_manager.h"

#include <cassert>

#include "engine/debug/logger.h"

namespace {
Transform& TransformOf(EcsManager& manager, Entity entity) {
    Transform* transform = manager.GetComponent<Transform>(entity);
    assert(transform);
    return *transform;
}

void TestSingleChildAndUnparent() {
    EcsManager manager;
    Entity parent = *manager.CreateEntity();
    Entity child = *manager.CreateEntity();

    manager.Parent(parent, child);
    assert(TransformOf(manager, parent).firstChildEntityId == child.entityId);
    assert(TransformOf(manager, child).parentEntityId == parent.entityId);
    assert(TransformOf(manager, child).nextSiblingEntityId == child.entityId);
    assert(TransformOf(manager, child).prevSiblingEntityId == child.entityId);

    manager.UnParent(child);
    assert(TransformOf(manager, parent).firstChildEntityId == 0);
    assert(TransformOf(manager, child).parentEntityId == 0);
    assert(TransformOf(manager, child).nextSiblingEntityId == 0);
    assert(TransformOf(manager, child).prevSiblingEntityId == 0);
}

void TestSiblingInsertionAndReparenting() {
    EcsManager manager;
    Entity firstParent = *manager.CreateEntity();
    Entity secondParent = *manager.CreateEntity();
    Entity first = *manager.CreateEntity();
    Entity second = *manager.CreateEntity();
    Entity third = *manager.CreateEntity();

    manager.Parent(firstParent, first);
    manager.Parent(firstParent, second);
    manager.Parent(firstParent, third);

    assert(TransformOf(manager, first).nextSiblingEntityId == second.entityId);
    assert(TransformOf(manager, first).prevSiblingEntityId == third.entityId);
    assert(TransformOf(manager, second).nextSiblingEntityId == third.entityId);
    assert(TransformOf(manager, second).prevSiblingEntityId == first.entityId);
    assert(TransformOf(manager, third).nextSiblingEntityId == first.entityId);
    assert(TransformOf(manager, third).prevSiblingEntityId == second.entityId);

    manager.Parent(secondParent, second);
    assert(TransformOf(manager, first).nextSiblingEntityId == third.entityId);
    assert(TransformOf(manager, first).prevSiblingEntityId == third.entityId);
    assert(TransformOf(manager, third).nextSiblingEntityId == first.entityId);
    assert(TransformOf(manager, third).prevSiblingEntityId == first.entityId);
    assert(TransformOf(manager, secondParent).firstChildEntityId == second.entityId);
    assert(TransformOf(manager, second).parentEntityId == secondParent.entityId);
}

void TestInvalidRelationshipsAreIgnored() {
    EcsManager manager;
    Entity parent = *manager.CreateEntity();
    Entity child = *manager.CreateEntity();

    manager.Parent(parent, child);
    manager.Parent(child, parent);
    manager.Parent(parent, parent);

    assert(TransformOf(manager, parent).parentEntityId == 0);
    assert(TransformOf(manager, parent).firstChildEntityId == child.entityId);
    assert(TransformOf(manager, child).parentEntityId == parent.entityId);
}
}

int main() {
    Logger::Info("ECSHierarchyTest", "Running ECS Hierarchy Tests");
    Logger::Info("ECSHierarchyTest", "Running TestSingleChildAndUnparent");
    TestSingleChildAndUnparent();
    Logger::Info("ECSHierarchyTest", "Running TestSiblingInsertionAndReparenting");
    TestSiblingInsertionAndReparenting();
    Logger::Info("ECSHierarchyTest", "Running TestInvalidRelationshipsAreIgnored");
    TestInvalidRelationshipsAreIgnored();
    Logger::Info("ECSHierarchyTest", "All ECS Hierarchy Tests Completed");
}
