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
        assert(CounterScript::destroys == 1);
        assert(CounterScript::destroyedValue == 10);
        assert(ecs.GetEntityComponentView<CounterScript>().size() == 1);
        ecs.DestroyEntity(second);
        assert(CounterScript::destroys == 2);
        assert(CounterScript::destroyedValue == 1);
        assert(OtherScript::destroys == 0);
        assert(ecs.GetEntityComponentView<CounterScript>().empty());
        ecs.DestroyEntity(first);
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
    assert(!scripts.GetScript<NoOpScript>(entity));
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
        assert(CounterScript::destroys == 1);
        assert(CounterScript::destroyedValue == 18);
    }
    assert(CounterScript::destroys == 1);
    assert(!ecs.GetComponent<CounterScript>(entity));
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
    TestRegistrationAfterExistingComponent();
    TestRegistrationPreservesSerialization();
    TestSceneSaveSkipsRuntimeScript();
}
