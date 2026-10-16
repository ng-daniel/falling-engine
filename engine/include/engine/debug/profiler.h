#pragma once

#include <chrono>
#include <cstddef>
#include <limits>
#include <string>
#include <vector>

/**
 * @brief Represents the resulting recording from a single ProfileScope
 * Is like a node in a tree, but the tree is represented like a vector
 */
struct ProfileSample {
    static constexpr std::size_t NO_PARENT = std::numeric_limits<std::size_t>::max();
    std::string name;
    std::size_t parentIndex = NO_PARENT;
    double startOffsetMs = 0.0;
    double durationMs = 0.0;
};

/**
 * @brief Represents one frame in its totality,
 * and a vector of all samples taken within the frame
 * in a tree-like representation thing with the indices
 */
struct ProfileFrame {
    double durationMs = 0.0;
    std::vector<ProfileSample> samples;
};

/**
 * @brief The main class for profiling
 * Stores the current and previous ProfileFrame
 * Handles additions  
 */
class Profiler {
public:
    Profiler() = default;
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;

    void BeginFrame();
    void EndFrame();

    /**
     * @brief Get the Last Frame object
     * The reference is only valid until the next EndFrame()
     * at which point it is overridden by the next frame
     *
     * Last frame is null until the first frame finishes
     * 
     * @return const ProfileFrame& 
     */
    const ProfileFrame& GetLastFrame() const { return lastFrame; }
    const void LogLastFrame() const;

private:
    
    ///
    /// SCOPE MANAGEMENT
    /// -------------------------

    // make friend so profileScopes can communicate
    // their creation and deletion via BeginScope and EndScope
    friend class ProfileScope;

    // type alias for clock bc thats a long nameee
    using Clock = std::chrono::steady_clock;

    // friends :3
    std::size_t BeginScope(std::string name);
    void EndScope(std::size_t sampleIndex, Clock::time_point startTime);

    ///
    /// FRAME STORAGE
    /// -------------------------
    
    ProfileFrame currentFrame;
    ProfileFrame lastFrame;

    // a stack representing the current profile scope stack
    // changes very very often
    std::vector<std::size_t> liveScopeStack;
    Clock::time_point frameStart; // start timestamp of current frame 
    bool recording = false; // true inbetween BeginFrame and EndFrame
};

class ProfileScope {
public:
    ProfileScope(Profiler& profiler, std::string name);
    ~ProfileScope();
private:
    Profiler& profiler;
    Profiler::Clock::time_point startTime;
    std::size_t sampleIndex; // idx of the ProfileSample struct associated
                             // with the scope object
};
