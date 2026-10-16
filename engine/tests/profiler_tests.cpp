#include "engine/debug/profiler.h"

#include <cassert>
#include <string>

namespace {
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
}

int main() {
    TestNestedScopesAndCompletedFrame();
    TestNextFrameReplacesCompletedFrame();
}
