#pragma once

#include "engine/ecs/ecs_structures.h"

class MeshRenderer : public IComponent {
public:
    std::string GetType() const override { return "MeshRenderer"; }
    UUID meshId;

    static void Serialize(JsonArchive& archive, const MeshRenderer& meshRenderer);
    static void Deserialize(JsonArchive& archive, MeshRenderer& meshRenderer);

    void SetMesh(UUID meshId);
};