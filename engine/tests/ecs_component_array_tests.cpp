#include "engine/ecs/ecs_warehouse.h"
#include "engine/ecs/components/transform.h"

#include <cassert>

namespace {
struct FirstComponent : Component {
    int value = 0;
    std::string GetType() const override { return "FirstComponent"; }
};

struct SecondComponent : Component {
    int value = 0;
    std::string GetType() const override { return "SecondComponent"; }
};

void TestQueriesBeforeCreation() {
    EcsWarehouse warehouse;
    Entity entity = *warehouse.CreateEntityNew();
    const EcsWarehouse& readOnly = warehouse;

    assert(warehouse.GetComponent<FirstComponent>(entity) == nullptr);
    assert(readOnly.GetComponentReadOnly<SecondComponent>(entity) == nullptr);

    // Create the higher-ID array first, leaving a gap for the first type.
    warehouse.AddComponent<SecondComponent>(entity)->value = 22;
    assert(!readOnly.HasComponent<FirstComponent>(entity));
    assert(warehouse.GetEntityComponentView<FirstComponent>().empty());
    warehouse.RemoveComponent<FirstComponent>(entity);
    warehouse.AddComponent<FirstComponent>(entity)->value = 11;

    assert(readOnly.GetComponentReadOnly<FirstComponent>(entity)->value == 11);
    assert(readOnly.GetComponentReadOnly<SecondComponent>(entity)->value == 22);
    warehouse.RemoveComponent<FirstComponent>(entity);
    assert(!readOnly.HasComponent<FirstComponent>(entity));
    assert(readOnly.GetComponentReadOnly<SecondComponent>(entity)->value == 22);
}

void TestSeparateWarehouses() {
    EcsWarehouse first;
    EcsWarehouse second;
    Entity firstEntity = *first.CreateEntityNew();
    Entity secondEntity = *second.CreateEntityNew();

    first.AddComponent<FirstComponent>(firstEntity)->value = 1;
    first.AddComponent<SecondComponent>(firstEntity)->value = 2;
    second.AddComponent<SecondComponent>(secondEntity)->value = 3;
    assert(second.GetComponent<FirstComponent>(secondEntity) == nullptr);
    second.AddComponent<FirstComponent>(secondEntity)->value = 4;

    assert(first.GetComponent<FirstComponent>(firstEntity)->value == 1);
    assert(first.GetComponent<SecondComponent>(firstEntity)->value == 2);
    assert(second.GetComponent<FirstComponent>(secondEntity)->value == 4);
    assert(second.GetComponent<SecondComponent>(secondEntity)->value == 3);
    second.DeleteEntity(secondEntity);
    assert(first.GetComponent<FirstComponent>(firstEntity)->value == 1);
}

void TestPagedDenseGrowthAndMappings() {
    EcsWarehouse warehouse;
    const Entity first = *warehouse.CreateEntityNew();
    const Entity second = *warehouse.CreateEntityNew();
    FirstComponent* firstComponent = warehouse.AddComponent<FirstComponent>(first);
    FirstComponent* secondComponent = warehouse.AddComponent<FirstComponent>(second);
    firstComponent->value = 1;
    secondComponent->value = 2;
    Transform* firstTransform = warehouse.GetComponent<Transform>(first);
    assert(firstTransform);

    Entity lastAdded = second;
    size_t visited = 0;
    auto initialView = warehouse.GetEntityComponentView<FirstComponent>();
    for (auto [runtimeId, component] : initialView) {
        if (runtimeId == first.entityRuntimeIdx) {
            for (int i = 0; i < 600; ++i) {
                lastAdded = *warehouse.CreateEntityNew();
                warehouse.AddComponent<FirstComponent>(lastAdded)->value = 1000 + i;
            }
            component.value = 41;
        }
        ++visited;
    }

    assert(visited == 2);
    assert(initialView.size() == 2);
    assert(firstComponent == warehouse.GetComponent<FirstComponent>(first));
    assert(secondComponent == warehouse.GetComponent<FirstComponent>(second));
    assert(firstTransform == warehouse.GetComponent<Transform>(first));
    assert(firstComponent->value == 41);
    assert(secondComponent->value == 2);

    auto fullView = warehouse.GetEntityComponentView<FirstComponent>();
    assert(fullView.size() == 602);
    for (auto [runtimeId, component] : fullView) {
        Entity* entity = warehouse.FindEntityByRuntimeId(runtimeId);
        assert(entity);
        assert(warehouse.GetComponent<FirstComponent>(*entity) == &component);
    }

    warehouse.RemoveComponent<FirstComponent>(second);
    assert(!warehouse.GetComponent<FirstComponent>(second));
    assert(warehouse.GetComponent<FirstComponent>(lastAdded)->value == 1599);
    auto afterRemoval = warehouse.GetEntityComponentView<FirstComponent>();
    assert(afterRemoval.size() == 601);
    for (auto [runtimeId, component] : afterRemoval) {
        Entity* entity = warehouse.FindEntityByRuntimeId(runtimeId);
        assert(entity);
        assert(warehouse.GetComponent<FirstComponent>(*entity) == &component);
    }
}
}

int main() {
    TestQueriesBeforeCreation();
    TestSeparateWarehouses();
    TestPagedDenseGrowthAndMappings();
}
