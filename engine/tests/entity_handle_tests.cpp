#include "engine/ecs/entity_handle.h"

#include <cassert>

namespace {
struct ValueComponent : Component {
    int value = 0;
    std::string GetType() const override { return "ValueComponent"; }
};

void CheckInvalid(const EntityHandle& handle) {
    assert(!handle.IsValid());
    assert(!static_cast<bool>(handle));
    assert(handle.GetComponent<ValueComponent>() == nullptr);
    assert(handle.GetComponentReadOnly<ValueComponent>() == nullptr);
    assert(!handle.HasComponent<ValueComponent>());
    assert(handle.AddComponent<ValueComponent>() == nullptr);
    handle.RemoveComponent<ValueComponent>();
    handle.Destroy();
}

void TestValidityAndComponents() {
    EcsManager ecs;
    CheckInvalid(EntityHandle{});
    CheckInvalid(EntityHandle(ecs, INVALID_UUID));
    CheckInvalid(EntityHandle(ecs, UUID{42}));

    const EntityHandle handle(ecs, ecs.CreateEntity()->entityId);
    assert(handle.IsValid());
    assert(static_cast<bool>(handle));
    assert(handle.HasComponent<Transform>());
    assert(!handle.HasComponent<ValueComponent>());
    assert(handle.GetComponent<ValueComponent>() == nullptr);
    handle.AddComponent<ValueComponent>()->value = 7;
    assert(handle.AddComponent<ValueComponent>() == nullptr);
    assert(handle.GetComponent<ValueComponent>()->value == 7);
    assert(handle.GetComponentReadOnly<ValueComponent>()->value == 7);
    handle.RemoveComponent<ValueComponent>();
    assert(!handle.HasComponent<ValueComponent>());
    handle.RemoveComponent<ValueComponent>();
    handle.Destroy();
    CheckInvalid(handle);
}

void TestStorageMovement() {
    EcsManager ecs;
    const EntityHandle first(ecs, ecs.CreateEntity()->entityId);
    const EntityHandle second(ecs, ecs.CreateEntity()->entityId);
    first.AddComponent<ValueComponent>()->value = 11;
    second.AddComponent<ValueComponent>()->value = 22;

    // Removing the first entry moves the second entry into its dense slot.
    first.RemoveComponent<ValueComponent>();
    assert(second.GetComponent<ValueComponent>()->value == 22);
    for (int i = 0; i < 256; ++i) {
        const EntityHandle added(ecs, ecs.CreateEntity()->entityId);
        added.AddComponent<ValueComponent>()->value = i;
    }
    assert(second.GetComponentReadOnly<ValueComponent>()->value == 22);
}

void TestIdentityAndReuse() {
    EcsManager ecs;
    EcsManager otherEcs;
    const Entity original = *ecs.CreateEntity(UUID{123}, "original");
    const EntityHandle handle(ecs, original.entityId);
    assert(handle.GetUUID() == original.entityId);
    assert(handle == EntityHandle(ecs, original.entityId));
    assert(handle != EntityHandle(otherEcs, original.entityId));
    assert(handle != EntityHandle{});

    handle.Destroy();
    const Entity replacement = *ecs.CreateEntity(UUID{456}, "replacement");
    assert(replacement.entityRuntimeIdx == original.entityRuntimeIdx);
    CheckInvalid(handle);
    assert(ecs.IsEntityAlive(replacement));

    ecs.CreateEntity(original.entityId, "recreated");
    assert(handle.IsValid());
    assert(handle.HasComponent<Transform>());
    handle.Destroy();
    CheckInvalid(handle);
}
}

int main() {
    TestValidityAndComponents();
    TestStorageMovement();
    TestIdentityAndReuse();
}
