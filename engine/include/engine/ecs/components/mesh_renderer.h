#pragma once

#include "engine/ecs/ecs_structures.h"

class MeshRenderer : public IComponent {
public:
    std::string GetType() const override { return "MeshRenderer"; }
    UUID meshId;
};