#include "engine/ecs/components/mesh_renderer.h"

void MeshRenderer::SetMesh(UUID meshId) {
    this->meshId = meshId;
}