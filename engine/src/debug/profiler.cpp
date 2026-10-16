#include "engine/debug/profiler.h"

#include <cassert>
#include <utility>

void Profiler::BeginFrame() {
    assert(!recording);
    assert(liveScopeStack.empty());

    currentFrame.samples.clear();
    currentFrame.durationMs = 0.0;
    frameStart = Clock::now();
    recording = true;
}

void Profiler::EndFrame() {
    assert(recording);
    assert(liveScopeStack.empty());

    currentFrame.durationMs = std::chrono::duration<double, std::milli>(
        Clock::now() - frameStart
    ).count();
    std::swap(currentFrame, lastFrame);
    recording = false;
}

std::size_t Profiler::BeginScope(std::string name) {
    assert(recording);

    Clock::time_point startTime = Clock::now();

    const std::size_t parentIndex = liveScopeStack.empty()
        ? ProfileSample::NO_PARENT
        : liveScopeStack.back();
    const std::size_t sampleIndex = currentFrame.samples.size();
    const double startOffsetMs = std::chrono::duration<double, std::milli>(
        startTime - frameStart
    ).count();

    currentFrame.samples.push_back({std::move(name), parentIndex, startOffsetMs, 0.0});
    liveScopeStack.push_back(sampleIndex);
    return sampleIndex;
}

void Profiler::EndScope(std::size_t sampleIndex, Clock::time_point startTime) {
    assert(recording);
    assert(!liveScopeStack.empty());
    assert(liveScopeStack.back() == sampleIndex);

    currentFrame.samples[sampleIndex].durationMs = std::chrono::duration<double, std::milli>(
        Clock::now() - startTime
    ).count();
    liveScopeStack.pop_back();
}

ProfileScope::ProfileScope(Profiler& profiler, std::string name)
    : profiler(profiler)
    , startTime(std::chrono::steady_clock::now())
    , sampleIndex(profiler.BeginScope(std::move(name))) {
}

ProfileScope::~ProfileScope() {
    profiler.EndScope(sampleIndex, startTime);
}
