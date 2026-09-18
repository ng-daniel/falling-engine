#pragma once

#include "engine/ecs/components/component.h"
#include "engine/utils/uuid.h"

class MeshRenderer : public Component {
public:
    std::string GetType() const override { return "MeshRenderer"; }
    UUID meshId;

    void SetMesh(UUID meshId);
};

class MeshRendererSerializer : public ComponentSerializer {
public:
    static void Serialize(JsonArchive& archive, const MeshRenderer& meshRenderer);
    static void Deserialize(JsonArchive& archive, MeshRenderer& meshRenderer);
};
