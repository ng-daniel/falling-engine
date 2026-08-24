#include "engine/ecs/ecs_component_registry.h"

void ECSComponentRegistry::RegisterComponent(
    std::string type,
    void* (*serializeFunc)(JsonArchive&, const IComponent&),
    IComponent* (*deserializeFunc)(void*)
) {
    componentRegistry[type] = {serializeFunc, deserializeFunc};
}

const ComponentInfo * ECSComponentRegistry::GetComponentInfo(const std::string& type) {
    auto it = componentRegistry.find(type);
    if (it != componentRegistry.end()) {
        return &(it->second);
    }
    return nullptr;
}