#pragma once

#include "engine/ecs/ecs_structures.h"

class MeshRenderer : public IComponent {
public:
    void SetMesh(UUID meshId);
    std::string GetType() const override { return "MeshRenderer"; }
    UUID meshId;
};