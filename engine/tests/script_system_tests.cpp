#include "engine/scripting/script_manager.h"
#include "engine/ecs/ecs_scene_service.h"

#include <cassert>
#include <string>

namespace {
struct CounterScript : Component {
    int value = 0;
    static inline int creates = 0;
    static inline int updates = 0;
    static inline int destroys = 0;
    static inline int createdValue = 0;
    static inline float lastDt = 0.0f;

    std::string GetType() const override { return "CounterScript"; }

    static void OnCreate(ScriptContext&, Entity, CounterScript& script) {
        ++creates;
        createdValue = script.value;
    }
    static void OnUpdate(ScriptContext&, Entity, CounterScript& script, float dt) {
        ++updates;
        ++script.value;
        lastDt = dt;
    }
    static void OnDestroy(ScriptContext&, Entity, CounterScript&) {
        ++destroys;
    }
};

struct OtherScript : Component {
    static inline int updates = 0;
    std::string GetType() const override { return "OtherScript"; }
    static void OnCreate(ScriptContext&, Entity, OtherScript&) {}
    static void OnUpdate(ScriptContext&, Entity, OtherScript&, float) { ++updates; }
    static void OnDestroy(ScriptContext&, Entity, OtherScript&) {}
};

void ResetCounters() {
    CounterScript::creates = 0;
    CounterScript::updates = 0;
    CounterScript::destroys = 0;
    CounterScript::createdValue = 0;
    CounterScript::lastDt = 0.0f;
    OtherScript::updates = 0;
}

void TestLifecycleAndDenseUpdates() {
    ResetCounters();
    EcsManager ecs;
    const Entity first = *ecs.CreateEntity();
    const Entity second = *ecs.CreateEntity();
    {
        ScriptManager scripts(ecs);
        scripts.RegisterScript<CounterScript>();
        scripts.RegisterScript<OtherScript>();

        CounterScript initial;
        initial.value = 9;
        assert(ecs.AddComponent<CounterScript>(first, initial));
        assert(CounterScript::createdValue == 9);
        assert(ecs.AddComponent<CounterScript>(second));
        assert(ecs.AddComponent<OtherScript>(first));
        assert(CounterScript::creates == 2);
        assert(CounterScript::createdValue == 0);

        scripts.Update(0.25f);
        assert(CounterScript::updates == 2);
        assert(CounterScript::lastDt == 0.25f);
        assert(OtherScript::updates == 1);
        assert(ecs.GetComponent<CounterScript>(first)->value == 10);
        assert(ecs.GetComponent<CounterScript>(second)->value == 1);

        ecs.RemoveComponent<CounterScript>(first);
        assert(CounterScript::destroys == 1);
        assert(ecs.GetEntityComponentView<CounterScript>().size() == 1);
        ecs.DestroyEntity(second);
        assert(CounterScript::destroys == 2);
        assert(ecs.GetEntityComponentView<CounterScript>().empty());
    }
    assert(CounterScript::destroys == 2);
}

void TestExistingComponentsAndShutdown() {
    ResetCounters();
    EcsManager ecs;
    const Entity entity = *ecs.CreateEntity();
    CounterScript initial;
    initial.value = 17;
    ecs.AddComponent<CounterScript>(entity, initial);
    {
        ScriptManager scripts(ecs);
        scripts.RegisterScript<CounterScript>();
        assert(CounterScript::creates == 1);
        assert(CounterScript::createdValue == 17);
        scripts.Update(1.0f);
    }
    assert(CounterScript::destroys == 1);
    assert(ecs.GetComponent<CounterScript>(entity)->value == 18);
    ecs.RemoveComponent<CounterScript>(entity);
    assert(CounterScript::destroys == 1);
}

void SerializeCounter(JsonArchive&, const CounterScript&) {}
void DeserializeCounter(JsonArchive&, CounterScript&) {}

void TestRegistrationPreservesSerialization() {
    ECSComponentRegistry registry;
    registry.RegisterSerialization<CounterScript>(SerializeCounter, DeserializeCounter);
    registry.RegisterLifecycle<CounterScript>([](Entity) {}, [](Entity) {});
    const ComponentInfo* info = registry.GetComponentInfo("CounterScript");
    assert(info && info->serializeFunc && info->deserializeFunc);
    assert(info->onCreateFunc && info->onDestroyFunc);

    registry.RegisterLifecycle<CounterScript>({}, {});
    assert(info->serializeFunc && info->deserializeFunc);
    assert(!info->onCreateFunc && !info->onDestroyFunc);

    ECSComponentRegistry scriptFirst;
    scriptFirst.RegisterLifecycle<CounterScript>([](Entity) {}, [](Entity) {});
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
    TestExistingComponentsAndShutdown();
    TestRegistrationPreservesSerialization();
    TestSceneSaveSkipsRuntimeScript();
}
