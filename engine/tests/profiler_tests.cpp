#include "engine/debug/profiler.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <string>

namespace {
    void TestIndexedGeometryCounts() {
        RenderStats stats;
        stats.AddIndexedTriangles(4, 6, 10);
        assert(stats.vertices == 40);
        assert(stats.indices == 60);
        assert(stats.triangles == 20);

        // Incomplete triangles are discarded independently for each instance.
        stats.AddIndexedTriangles(5, 7, 2);
        assert(stats.vertices == 50);
        assert(stats.indices == 74);
        assert(stats.triangles == 24);

        stats.AddIndexedTriangles(4, 6, 0);
        stats.AddIndexedTriangles(4, 0, 10);
        assert(stats.vertices == 50);
        assert(stats.indices == 74);
        assert(stats.triangles == 24);
    }

    void TestGeometryCountsUse64BitArithmetic() {
        const std::uint32_t count = std::numeric_limits<std::uint32_t>::max();
        RenderStats stats;
        stats.AddIndexedTriangles(count, count, 3);
        assert(stats.vertices == 12'884'901'885ULL);
        assert(stats.indices == 12'884'901'885ULL);
        assert(stats.triangles == 4'294'967'295ULL);

        Profiler profiler;
        profiler.BeginFrame();
        profiler.AddRenderStats(stats);
        profiler.AddRenderStats(stats);
        profiler.EndFrame();
        const RenderStats& total = profiler.GetLastFrame().renderStats;
        assert(total.vertices == 25'769'803'770ULL);
        assert(total.indices == 25'769'803'770ULL);
        assert(total.triangles == 8'589'934'590ULL);
    }

    void TestFrameRenderStats() {
        Profiler profiler;
        const RenderStats& initial = profiler.GetLastFrame().renderStats;
        assert(initial.vertices == 0);
        assert(initial.indices == 0);
        assert(initial.triangles == 0);

        profiler.BeginFrame();
        profiler.AddRenderStats({40, 60, 20});
        profiler.AddRenderStats({3, 3, 1});
        profiler.EndFrame();
        const RenderStats& completed = profiler.GetLastFrame().renderStats;
        assert(completed.vertices == 43);
        assert(completed.indices == 63);
        assert(completed.triangles == 21);

        // Last frame stays available while the next frame is being recorded.
        profiler.BeginFrame();
        profiler.AddRenderStats({4, 6, 2});
        assert(profiler.GetLastFrame().renderStats.vertices == 43);
        assert(profiler.GetLastFrame().renderStats.indices == 63);
        assert(profiler.GetLastFrame().renderStats.triangles == 21);
        profiler.EndFrame();
        assert(profiler.GetLastFrame().renderStats.vertices == 4);
        assert(profiler.GetLastFrame().renderStats.indices == 6);
        assert(profiler.GetLastFrame().renderStats.triangles == 2);

        // Both reused frame buffers must reset, including frames with no draws.
        for (int i = 0; i < 2; ++i) {
            profiler.BeginFrame();
            profiler.EndFrame();
            assert(profiler.GetLastFrame().renderStats.vertices == 0);
            assert(profiler.GetLastFrame().renderStats.indices == 0);
            assert(profiler.GetLastFrame().renderStats.triangles == 0);
        }
    }

    void TestNestedScopesAndCompletedFrame() {
        Profiler profiler;
        assert(profiler.GetLastFrame().samples.empty());

        profiler.BeginFrame();
        {
            ProfileScope outer(profiler, "Render");
            {
                ProfileScope inner(profiler, "Build commands");
            }
        }
        {
            ProfileScope sibling(profiler, "Present");
        }
        profiler.EndFrame();

        const ProfileFrame& frame = profiler.GetLastFrame();
        assert(frame.samples.size() == 3);
        assert(frame.samples[0].name == "Render");
        assert(frame.samples[1].name == "Build commands");
        assert(frame.samples[2].name == "Present");
        assert(frame.samples[0].parentIndex == ProfileSample::NO_PARENT);
        assert(frame.samples[1].parentIndex == 0);
        assert(frame.samples[2].parentIndex == ProfileSample::NO_PARENT);

        for (const ProfileSample& sample : frame.samples) {
            assert(sample.startOffsetMs >= 0.0);
            assert(sample.durationMs >= 0.0);
            assert(sample.startOffsetMs + sample.durationMs <= frame.durationMs);
        }
    }

    void TestNextFrameReplacesCompletedFrame() {
        Profiler profiler;
        std::string name = "First";
        profiler.BeginFrame();
        {
            ProfileScope scope(profiler, name);
        }
        name = "Changed";
        profiler.EndFrame();
        assert(profiler.GetLastFrame().samples[0].name == "First");

        profiler.BeginFrame();
        assert(profiler.GetLastFrame().samples.size() == 1);
        profiler.EndFrame();
        assert(profiler.GetLastFrame().samples.empty());
        assert(profiler.GetLastFrame().durationMs >= 0.0);
    }

    void TestLastFrameFPS() {
        Profiler profiler;
        assert(profiler.GetLastFrameFPS() == 0.0);

        profiler.BeginFrame();
        profiler.EndFrame();

        const double durationMs = profiler.GetLastFrame().durationMs;
        if (durationMs > 0.0) {
            assert(std::abs(profiler.GetLastFrameFPS() * durationMs - 1000.0) < 0.000001);
        } else {
            assert(profiler.GetLastFrameFPS() == 0.0);
        }
    }
}

int main() {
    TestIndexedGeometryCounts();
    TestGeometryCountsUse64BitArithmetic();
    TestFrameRenderStats();
    TestNestedScopesAndCompletedFrame();
    TestNextFrameReplacesCompletedFrame();
    TestLastFrameFPS();
}
