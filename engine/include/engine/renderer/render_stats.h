#pragma once

#include <cstdint>

// Submitted geometry before GPU clipping, culling, and depth testing.
struct RenderStats {
    // Stored vertices per drawn primitive, including any unused entries.
    // This does not measure vertex shader invocations.
    std::uint64_t vertices = 0;
    std::uint64_t indices = 0;
    std::uint64_t triangles = 0;

    void AddIndexedTriangles(
        std::uint64_t vertexCount,
        std::uint64_t indexCount,
        std::uint64_t instanceCount
    ) noexcept {
        if (indexCount == 0) {
            return;
        }
        vertices += vertexCount * instanceCount;
        indices += indexCount * instanceCount;
        triangles += (indexCount / 3) * instanceCount;
    }
};
