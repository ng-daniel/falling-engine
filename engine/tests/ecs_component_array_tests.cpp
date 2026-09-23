#include "engine/ecs/ecs_warehouse.h"

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
}

int main() {
    TestQueriesBeforeCreation();
    TestSeparateWarehouses();
}
