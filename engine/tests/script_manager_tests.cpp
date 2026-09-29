#include "engine/scripting/script_manager.h"

#include <cassert>
#include <stdexcept>

namespace {
struct Counts {
    int awake = 0;
    int start = 0;
    int update = 0;
    int destroy = 0;
};

Counts firstCounts;
Counts secondCounts;

struct FirstScript : Script {
    void OnAwake() override {
        assert(GetCore().IsValid());
        ++firstCounts.awake;
    }
    void OnStart() override { ++firstCounts.start; }
    void OnUpdate(float deltaTime) override {
        assert(deltaTime == 0.5f);
        ++firstCounts.update;
    }
    void OnDestroy() override { ++firstCounts.destroy; }
};

struct SecondScript : Script {
    void OnAwake() override { ++secondCounts.awake; }
    void OnStart() override { ++secondCounts.start; }
    void OnUpdate(float) override { ++secondCounts.update; }
    void OnDestroy() override { ++secondCounts.destroy; }
};

ScriptManager* activeManager = nullptr;
struct MutatingScript : Script {
    void OnUpdate(float) override { activeManager->RemoveScript<MutatingScript>(GetCore()); }
};

void TestLifecycle() {
    EcsManager ecs;
    ScriptManager manager(ecs);
    manager.RegisterScript<FirstScript>();
    manager.RegisterScript<FirstScript>();
    manager.RegisterScript<SecondScript>();

    const EntityHandle first(ecs, ecs.CreateEntity()->entityId);
    const EntityHandle second(ecs, ecs.CreateEntity()->entityId);
    manager.AddScript<FirstScript>(first);
    manager.AddScript<FirstScript>(first);
    manager.AddScript<SecondScript>(first);
    manager.AddScript<FirstScript>(second);
    assert(firstCounts.awake == 2 && firstCounts.start == 2);
    assert(secondCounts.awake == 1 && secondCounts.start == 1);
    assert(first.HasComponent<FirstScript>());

    manager.Update(0.5f);
    assert(firstCounts.update == 2 && secondCounts.update == 1);
    manager.RemoveScript<FirstScript>(first);
    manager.RemoveScript<FirstScript>(first);
    assert(firstCounts.destroy == 1 && !first.HasComponent<FirstScript>());
    manager.RemoveAllScripts(first);
    assert(secondCounts.destroy == 1 && !first.HasComponent<SecondScript>());
    manager.DestroyEntity(second);
    assert(!second.IsValid() && firstCounts.destroy == 2);

    manager.AddScript<FirstScript>(first);
    const EntityHandle third(ecs, ecs.CreateEntity()->entityId);
    manager.AddScript<FirstScript>(third);
    manager.Shutdown();
    manager.Shutdown();
    assert(firstCounts.destroy == 4);
    assert(!first.HasComponent<FirstScript>() && !third.HasComponent<FirstScript>());
    bool rejected = false;
    try {
        manager.AddScript<FirstScript>(first);
    } catch (const std::logic_error&) {
        rejected = true;
    }
    assert(rejected);
}

void TestCallbackMutationGuard() {
    EcsManager ecs;
    ScriptManager manager(ecs);
    manager.RegisterScript<MutatingScript>();
    const EntityHandle owner(ecs, ecs.CreateEntity()->entityId);
    manager.AddScript<MutatingScript>(owner);
    activeManager = &manager;
    bool rejected = false;
    try {
        manager.Update(0.5f);
    } catch (const std::logic_error&) {
        rejected = true;
    }
    activeManager = nullptr;
    assert(rejected && owner.HasComponent<MutatingScript>());
    manager.RemoveScript<MutatingScript>(owner);
}
}

int main() {
    TestLifecycle();
    TestCallbackMutationGuard();
}
