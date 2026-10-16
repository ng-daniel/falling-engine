#include "engine/debug/profiler.h"
#include "engine/debug/logger.h"

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

double Profiler::GetLastFrameFPS() const noexcept {
    return lastFrame.durationMs > 0.0 ? 1000.0 / lastFrame.durationMs : 0.0;
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

    currentFrame.samples.push_back(
        {std::move(name),
             parentIndex,
             startOffsetMs,
             0.0
        }
    );
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

const void Profiler::LogLastFrame() const {
    Logger::Info("Profiler", "FRAME PERFORMANCE DATA:");
    Logger::Info("Profiler", "----------------------------------------");
    Logger::Info("Profiler", "Duration: " + std::to_string(lastFrame.durationMs) + " ms");
    Logger::Info("Profiler", "Inst. FPS: " + std::to_string(GetLastFrameFPS()));
    for (const auto& sample : lastFrame.samples) {
        std::string indent;
        std::size_t parentIndex = sample.parentIndex;
        while (parentIndex != ProfileSample::NO_PARENT) {
            indent += "  ";
            parentIndex = lastFrame.samples[parentIndex].parentIndex;
        }
        Logger::Info("Profiler", indent + sample.name + ": " + std::to_string(sample.durationMs) + " ms");
    }
    Logger::Info("Profiler", "----------------------------------------");
}

ProfileScope::ProfileScope(Profiler& profiler, std::string name)
    : profiler(profiler)
    , startTime(std::chrono::steady_clock::now())
    , sampleIndex(profiler.BeginScope(std::move(name))) {
}

ProfileScope::~ProfileScope() {
    profiler.EndScope(sampleIndex, startTime);
}
