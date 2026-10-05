#include "engine/scripting/script_manager.h"
#include "engine/ecs/ecs_scene_service.h"
#include "engine/utils/time.h"

#include <cassert>
#include <string>

namespace {
class CounterRuntime;

struct CounterScript : ScriptData {
    using Runtime = CounterRuntime;
    int value = 0;
    static inline int creates = 0;
    static inline int updates = 0;
    static inline int destroys = 0;
    static inline int createdValue = 0;
    static inline int destroyedValue = 0;
    static inline float lastDt = 0.0f;

    std::string GetType() const override { return "CounterScript"; }
};

class CounterRuntime : public ScriptRuntime {
public:
    static inline int constructions = 0;
    CounterRuntime() { ++constructions; }

    void OnCreate(ScriptContext&, Entity, ScriptData& data) override {
        ++CounterScript::creates;
        CounterScript::createdValue = static_cast<CounterScript&>(data).value;
    }
    void OnUpdate(ScriptContext&, Entity, ScriptData& data) override {
        ++CounterScript::updates;
        ++static_cast<CounterScript&>(data).value;
        CounterScript::lastDt = Time::GetDeltaTime();
    }
    void OnDestroy(ScriptContext&, Entity, ScriptData& data) override {
        ++CounterScript::destroys;
        CounterScript::destroyedValue = static_cast<CounterScript&>(data).value;
    }
};

struct OtherScript : ScriptData {
    static inline int updates = 0;
    static inline int destroys = 0;
    std::string GetType() const override { return "OtherScript"; }
};

class OtherRuntime : public ScriptRuntime {
public:
    void OnUpdate(ScriptContext&, Entity, ScriptData&) override { ++OtherScript::updates; }
    void OnDestroy(ScriptContext&, Entity, ScriptData&) override { ++OtherScript::destroys; }
};

void ResetCounters() {
    CounterRuntime::constructions = 0;
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
        scripts.RegisterScript<CounterScript, CounterRuntime>();
        scripts.RegisterScript<OtherScript, OtherRuntime>();
        assert(CounterRuntime::constructions == 1);

        CounterScript initial;
        initial.value = 9;
        assert(ecs.AddComponent<CounterScript>(first, initial));
        assert(CounterScript::createdValue == 9);
        assert(ecs.AddComponent<CounterScript>(second));
        assert(ecs.AddComponent<OtherScript>(first));
        assert(CounterScript::creates == 2);
        assert(CounterScript::createdValue == 0);
        assert(CounterRuntime::constructions == 1);

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
    assert(scripts.AddScript<CounterScript>(entity, initial));
    assert(CounterRuntime::constructions == 1);
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

void TestRegistrationAfterExistingComponent() {
    ResetCounters();
    EcsManager ecs;
    const Entity entity = *ecs.CreateEntity();
    CounterScript initial;
    initial.value = 17;
    ecs.AddComponent<CounterScript>(entity, initial);
    {
        ScriptManager scripts(ecs);
        scripts.RegisterScript<CounterScript, CounterRuntime>();
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
    scripts.RegisterScript<CounterScript, CounterRuntime>();
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
    TestRegistrationAfterExistingComponent();
    TestRegistrationPreservesSerialization();
    TestSceneSaveSkipsRuntimeScript();
}
