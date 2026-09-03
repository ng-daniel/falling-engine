#pragma once

struct GLFWwindow;

/**
 * @brief Owns the OS window, GL context, and ImGui context/backends.
 * Editor drives the frame loop by calling BeginFrame()/EndFrame() and drawing in between.
 */
class EditorWindow {
public:
    EditorWindow();
    ~EditorWindow();

    EditorWindow(const EditorWindow&) = delete;
    EditorWindow& operator=(const EditorWindow&) = delete;

    bool ShouldClose() const;

    /// @brief Polls OS events and starts a new ImGui frame.
    void BeginFrame();

    /// @brief Renders ImGui and presents the frame.
    void EndFrame();

private:
    GLFWwindow* handle = nullptr;
};
