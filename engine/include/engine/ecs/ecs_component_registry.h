#pragma once

#include <string>
#include <unordered_map>
#include "engine/ecs/components/transform.h"

/**
 * @brief Util class with general component registry functionality
 */
struct ComponentInfo {
    void* (*serializeFunc)(JsonArchive&, const IComponent&);
    IComponent* (*deserializeFunc)(void*);
};

class ECSComponentRegistry {
public:
    void RegisterComponent(
        std::string type,
        void* (*serializeFunc)(JsonArchive&, const IComponent&),
        IComponent* (*deserializeFunc)(void*)
    );
    const ComponentInfo * GetComponentInfo(const std::string& type);

private:
    std::unordered_map<std::string, ComponentInfo> componentRegistry;
};