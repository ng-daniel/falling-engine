#include "engine/ecs/components/mesh_renderer.h"
#include "engine/serialization/uuid_serializer.h"
#include "engine/utils/uuid.h"

void MeshRenderer::SetMesh(UUID meshId) {
    this->meshId = meshId;
}
void MeshRenderer::Serialize(JsonArchive& archive, const MeshRenderer& meshRenderer) {
    archive.BeginObject();
    UUIDSerializer::Serialize(archive, meshRenderer.meshId, "meshId");
    archive.EndObject();
}

void MeshRenderer::Deserialize(JsonArchive& archive, MeshRenderer& meshRenderer) {
    archive.BeginObject();
    UUIDSerializer::Deserialize(archive, meshRenderer.meshId, "meshId");
    archive.EndObject();
}