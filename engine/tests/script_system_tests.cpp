#include "engine/scripting/script_manager.h"
#include "engine/ecs/ecs_scene_service.h"
#include "engine/utils/time.h"

#include <cassert>
#include <string>

namespace {
struct CounterScript : ScriptData {
    int value = 0;
    static inline int creates = 0;
    static inline int updates = 0;
    static inline int destroys = 0;
    static inline int createdValue = 0;
    static inline int destroyedValue = 0;
    static inline float lastDt = 0.0f;

    std::string GetType() const override { return "CounterScript"; }

    static void OnCreate(ScriptContext&, Entity, CounterScript& data) {
        ++CounterScript::creates;
        CounterScript::createdValue = data.value;
    }
    static void OnUpdate(ScriptContext&, Entity, CounterScript& data) {
        ++CounterScript::updates;
        ++data.value;
        CounterScript::lastDt = Time::GetDeltaTime();
    }
    static void OnDestroy(ScriptContext&, Entity, CounterScript& data) {
        ++CounterScript::destroys;
        CounterScript::destroyedValue = data.value;
    }
};

struct OtherScript : ScriptData {
    static inline int updates = 0;
    static inline int destroys = 0;
    std::string GetType() const override { return "OtherScript"; }

    static void OnUpdate(ScriptContext&, Entity, OtherScript&) { ++OtherScript::updates; }
    static void OnDestroy(ScriptContext&, Entity, OtherScript&) { ++OtherScript::destroys; }
};

struct NoOpScript : ScriptData {
    std::string GetType() const override { return "NoOpScript"; }
};

struct ValueComponent : Component {
    int value = 0;
    std::string GetType() const override { return "ValueComponent"; }
};

struct ReferenceScript : ScriptData {
    UUID targetEntityId = INVALID_UUID;
    int ownValue = -1;
    int targetValue = -1;
    std::string GetType() const override { return "ReferenceScript"; }

    static void OnUpdate(ScriptContext& context, Entity entity, ReferenceScript& data) {
        if (ValueComponent* own = context.GetComponent<ValueComponent>(entity)) {
            data.ownValue = own->value;
        }
        if (ValueComponent* target = context.GetComponent<ValueComponent>(data.targetEntityId)) {
            data.targetValue = target->value;
        } else {
            data.targetValue = -1;
        }
    }
};

struct RemoveDuringUpdateScript : ScriptData {
    static inline UUID targetEntityId = INVALID_UUID;
    static inline UUID cleanupEntityId = INVALID_UUID;
    static inline int updates = 0;
    static inline int destroys = 0;
    std::string GetType() const override { return "RemoveDuringUpdateScript"; }

    static void OnUpdate(ScriptContext& context, Entity entity, RemoveDuringUpdateScript&) {
        ++updates;
        context.ecs.RemoveComponent<RemoveDuringUpdateScript>(entity);
        if (entity.entityId != targetEntityId) {
            if (Entity* target = context.ecs.GetEntity(targetEntityId)) {
                context.ecs.DestroyEntity(*target);
            }
        }
    }

    static void OnDestroy(ScriptContext& context, Entity, RemoveDuringUpdateScript&) {
        ++destroys;
        if (Entity* cleanup = context.ecs.GetEntity(cleanupEntityId)) {
            context.ecs.DestroyEntity(*cleanup);
        }
    }
};

struct LateRegisteredScript : ScriptData {
    static inline int creates = 0;
    static inline int updates = 0;
    std::string GetType() const override { return "LateRegisteredScript"; }
    static void OnCreate(ScriptContext&, Entity, LateRegisteredScript&) { ++creates; }
    static void OnUpdate(ScriptContext&, Entity, LateRegisteredScript&) { ++updates; }
};

struct AddsScriptOnUpdate : ScriptData {
    static inline ScriptManager* scripts = nullptr;
    static inline int updates = 0;
    bool added = false;
    std::string GetType() const override { return "AddsScriptOnUpdate"; }

    static void OnUpdate(ScriptContext&, Entity entity, AddsScriptOnUpdate& data) {
        ++updates;
        if (!data.added) {
            data.added = true;
            LateRegisteredScript* added = scripts->AddScript<LateRegisteredScript>(entity);
            assert(added);
        }
    }
};

struct TrailingScript : ScriptData {
    static inline int updates = 0;
    std::string GetType() const override { return "TrailingScript"; }
    static void OnUpdate(ScriptContext&, Entity, TrailingScript&) { ++updates; }
};

void ResetCounters() {
    CounterScript::creates = 0;
    CounterScript::updates = 0;
    CounterScript::destroys = 0;
    CounterScript::createdValue = 0;
    CounterScript::destroyedValue = 0;
    CounterScript::lastDt = 0.0f;
    OtherScript::updates = 0;
    OtherScript::destroys = 0;
}

void TestLifecycleAndDenseUpdates() {
    ResetCounters();
    EcsManager ecs;
    const Entity first = *ecs.CreateEntity();
    const Entity second = *ecs.CreateEntity();
    {
        ScriptManager scripts(ecs);
        const bool counterRegistered = scripts.RegisterScript<CounterScript>();
        const bool otherRegistered = scripts.RegisterScript<OtherScript>();
        assert(counterRegistered);
        assert(otherRegistered);
        assert(!scripts.RegisterScript<CounterScript>());

        CounterScript initial;
        initial.value = 9;
        assert(ecs.AddComponent<CounterScript>(first, initial));
        assert(CounterScript::createdValue == 9);
        assert(ecs.AddComponent<CounterScript>(second));
        assert(ecs.AddComponent<OtherScript>(first));
        assert(CounterScript::creates == 2);
        assert(CounterScript::createdValue == 0);

        Time::Reset();
        Time::Update();
        const float frameDt = Time::GetDeltaTime();
        scripts.UpdateAllScripts();
        assert(CounterScript::updates == 2);
        assert(CounterScript::lastDt == frameDt);
        assert(OtherScript::updates == 1);
        assert(ecs.GetComponent<CounterScript>(first)->value == 10);
        assert(ecs.GetComponent<CounterScript>(second)->value == 1);

        ecs.RemoveComponent<CounterScript>(first);
        assert(CounterScript::destroys == 0);
        assert(ecs.GetEntityComponentView<CounterScript>().size() == 2);
        ecs.FlushRemovals();
        assert(CounterScript::destroys == 1);
        assert(CounterScript::destroyedValue == 10);
        assert(ecs.GetEntityComponentView<CounterScript>().size() == 1);
        ecs.DestroyEntity(second);
        assert(ecs.IsEntityAlive(second));
        ecs.FlushRemovals();
        assert(CounterScript::destroys == 2);
        assert(CounterScript::destroyedValue == 1);
        assert(OtherScript::destroys == 0);
        assert(ecs.GetEntityComponentView<CounterScript>().empty());
        ecs.DestroyEntity(first);
        ecs.FlushRemovals();
        assert(OtherScript::destroys == 1);
    }
    assert(CounterScript::destroys == 2);
}

void TestScriptManagerProxies() {
    ResetCounters();
    EcsManager ecs;
    const Entity entity = *ecs.CreateEntity();
    ScriptManager scripts(ecs);

    CounterScript initial;
    initial.value = 4;
    CounterScript* added = scripts.AddScript<CounterScript>(entity, initial);
    assert(added);
    assert(!scripts.RegisterScript<CounterScript>());
    assert(CounterScript::creates == 1);
    assert(CounterScript::createdValue == 4);
    assert(scripts.GetScript<CounterScript>(entity)->value == 4);

    scripts.UpdateAllScripts();
    assert(CounterScript::updates == 1);
    assert(scripts.GetScript<CounterScript>(entity)->value == 5);

    scripts.RemoveScript<CounterScript>(entity);
    assert(CounterScript::destroys == 0);
    ecs.FlushRemovals();
    assert(CounterScript::destroys == 1);
    assert(CounterScript::destroyedValue == 5);
    assert(!scripts.GetScript<CounterScript>(entity));
}

void TestInheritedNoOpCallbacks() {
    EcsManager ecs;
    const Entity entity = *ecs.CreateEntity();
    ScriptManager scripts(ecs);

    NoOpScript* data = scripts.AddScript<NoOpScript>(entity);
    assert(data);
    scripts.UpdateAllScripts();
    scripts.RemoveScript<NoOpScript>(entity);
    ecs.FlushRemovals();
    assert(!scripts.GetScript<NoOpScript>(entity));
}

void TestComponentAccessFromScript() {
    EcsManager ecs;
    const Entity owner = *ecs.CreateEntity();
    const Entity target = *ecs.CreateEntity();
    ecs.AddComponent<ValueComponent>(owner)->value = 7;
    ecs.AddComponent<ValueComponent>(target)->value = 12;

    ScriptManager scripts(ecs);
    ReferenceScript initial;
    initial.targetEntityId = target.entityId;
    ReferenceScript* script = scripts.AddScript<ReferenceScript>(owner, initial);
    assert(script);

    scripts.UpdateAllScripts();
    assert(scripts.GetScript<ReferenceScript>(owner)->ownValue == 7);
    assert(scripts.GetScript<ReferenceScript>(owner)->targetValue == 12);

    ecs.DestroyEntity(target);
    ecs.FlushRemovals();
    const Entity replacement = *ecs.CreateEntity();
    assert(replacement.entityRuntimeIdx == target.entityRuntimeIdx);
    ecs.AddComponent<ValueComponent>(replacement)->value = 99;
    scripts.UpdateAllScripts();
    assert(scripts.GetScript<ReferenceScript>(owner)->targetValue == -1);
}

void TestRegistrationAfterExistingComponent() {
    ResetCounters();
    EcsManager ecs;
    const Entity entity = *ecs.CreateEntity();
    CounterScript initial;
    initial.value = 17;
    ecs.AddComponent<CounterScript>(entity, initial);
    {
        ScriptManager scripts(ecs);
        scripts.RegisterScript<CounterScript>();
        assert(CounterScript::creates == 0);
        scripts.UpdateAllScripts();
        assert(CounterScript::updates == 1);
        assert(ecs.GetComponent<CounterScript>(entity)->value == 18);
        ecs.RemoveComponent<CounterScript>(entity);
        ecs.FlushRemovals();
        assert(CounterScript::destroys == 1);
        assert(CounterScript::destroyedValue == 18);
    }
    assert(CounterScript::destroys == 1);
    assert(!ecs.GetComponent<CounterScript>(entity));
}

void TestDeferredRemovalDuringUpdate() {
    RemoveDuringUpdateScript::updates = 0;
    RemoveDuringUpdateScript::destroys = 0;
    EcsManager ecs;
    ScriptManager scripts(ecs);
    const Entity first = *ecs.CreateEntity();
    const Entity second = *ecs.CreateEntity();
    const Entity cleanup = *ecs.CreateEntity();
    RemoveDuringUpdateScript::targetEntityId = second.entityId;
    RemoveDuringUpdateScript::cleanupEntityId = cleanup.entityId;
    RemoveDuringUpdateScript* firstData = scripts.AddScript<RemoveDuringUpdateScript>(first);
    RemoveDuringUpdateScript* secondData = scripts.AddScript<RemoveDuringUpdateScript>(second);
    assert(firstData && secondData);

    scripts.UpdateAllScripts();
    assert(RemoveDuringUpdateScript::updates == 2);
    assert(RemoveDuringUpdateScript::destroys == 0);
    assert(firstData == scripts.GetScript<RemoveDuringUpdateScript>(first));
    assert(secondData == scripts.GetScript<RemoveDuringUpdateScript>(second));
    assert(ecs.IsEntityAlive(second));
    ecs.DestroyEntity(second); // duplicate request must not invoke OnDestroy twice
    const Entity added = *ecs.CreateEntity();
    ValueComponent* addedValue = ecs.AddComponent<ValueComponent>(added);
    assert(addedValue);
    addedValue->value = 42;

    ecs.FlushRemovals();
    assert(RemoveDuringUpdateScript::destroys == 2);
    assert(!scripts.GetScript<RemoveDuringUpdateScript>(first));
    assert(!ecs.IsEntityAlive(second));
    assert(!ecs.IsEntityAlive(cleanup));
    assert(ecs.GetEntityComponentView<RemoveDuringUpdateScript>().empty());
    assert(ecs.GetComponent<ValueComponent>(added) == addedValue);
    assert(addedValue->value == 42);
}

void TestRegistrationDuringUpdate() {
    AddsScriptOnUpdate::updates = 0;
    TrailingScript::updates = 0;
    LateRegisteredScript::creates = 0;
    LateRegisteredScript::updates = 0;

    EcsManager ecs;
    ScriptManager scripts(ecs);
    AddsScriptOnUpdate::scripts = &scripts;
    const Entity entity = *ecs.CreateEntity();
    AddsScriptOnUpdate* source = scripts.AddScript<AddsScriptOnUpdate>(entity);
    TrailingScript* trailing = scripts.AddScript<TrailingScript>(entity);
    assert(source && trailing);

    scripts.UpdateAllScripts();
    assert(AddsScriptOnUpdate::updates == 1);
    assert(TrailingScript::updates == 1);
    assert(LateRegisteredScript::creates == 1);
    assert(LateRegisteredScript::updates == 0);
    assert(scripts.GetScript<LateRegisteredScript>(entity));

    scripts.UpdateAllScripts();
    assert(AddsScriptOnUpdate::updates == 2);
    assert(TrailingScript::updates == 2);
    assert(LateRegisteredScript::updates == 1);
    AddsScriptOnUpdate::scripts = nullptr;
}

void SerializeCounter(JsonArchive&, const CounterScript&) {}
void DeserializeCounter(JsonArchive&, CounterScript&) {}

void TestRegistrationPreservesSerialization() {
    ECSComponentRegistry registry;
    registry.RegisterSerialization<CounterScript>(SerializeCounter, DeserializeCounter);
    registry.RegisterLifecycle<CounterScript>([](Entity, CounterScript*) {}, [](Entity, CounterScript*) {});
    const ComponentInfo* info = registry.GetComponentInfo("CounterScript");
    assert(info && info->serializeFunc && info->deserializeFunc);
    assert(info->onCreateFunc && info->onDestroyFunc);

    registry.RegisterLifecycle<CounterScript>({}, {});
    assert(info->serializeFunc && info->deserializeFunc);
    assert(!info->onCreateFunc && !info->onDestroyFunc);

    ECSComponentRegistry scriptFirst;
    scriptFirst.RegisterLifecycle<CounterScript>([](Entity, CounterScript*) {}, [](Entity, CounterScript*) {});
    scriptFirst.RegisterSerialization<CounterScript>(SerializeCounter, DeserializeCounter);
    info = scriptFirst.GetComponentInfo("CounterScript");
    assert(info && info->serializeFunc && info->deserializeFunc);
    assert(info->onCreateFunc && info->onDestroyFunc);
}

void TestSceneSaveSkipsRuntimeScript() {
    EcsManager ecs;
    ecs.RegisterComponents();
    const Entity entity = *ecs.CreateEntity();
    ScriptManager scripts(ecs);
    scripts.RegisterScript<CounterScript>();
    ecs.AddComponent<CounterScript>(entity);

    const SceneAsset scene = ECSSceneService::BuildSceneFromEntities(ecs, entity.entityId);
    assert(scene.entities.size() == 1);
    assert(scene.entities[0].components.size() == 1);
    assert(scene.entities[0].components[0].type == "Transform");
}
}

int main() {
    TestLifecycleAndDenseUpdates();
    TestScriptManagerProxies();
    TestInheritedNoOpCallbacks();
    TestComponentAccessFromScript();
    TestRegistrationAfterExistingComponent();
    TestDeferredRemovalDuringUpdate();
    TestRegistrationDuringUpdate();
    TestRegistrationPreservesSerialization();
    TestSceneSaveSkipsRuntimeScript();
}
