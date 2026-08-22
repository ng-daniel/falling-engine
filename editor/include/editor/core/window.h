#pragma once

struct GLFWwindow;

/**
 * @brief Owns the OS window, GL context, and ImGui context/backends.
 * Editor drives the frame loop by calling BeginFrame()/EndFrame() and drawing in between.
 */
class Window {
public:
    Window();
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool ShouldClose() const;

    /// @brief Polls OS events and starts a new ImGui frame.
    void BeginFrame();

    /// @brief Renders ImGui and presents the frame.
    void EndFrame();

private:
    GLFWwindow* handle = nullptr;
};
