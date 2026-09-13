#pragma once

#include <functional>

struct GLFWwindow;
class InputManager;

class WindowManager {
public:
    ~WindowManager();

    bool Init(InputManager& inputManager, const std::function<void()>& configure = {});
    bool ShouldClose() const;
    void ForceClose();
    void EndFrame();
    void Close();

    void GetFramebufferSize(int& width, int& height) const;
private:
    static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    static void CursorPositionCallback(GLFWwindow* window, double xPosition, double yPosition);

    GLFWwindow* handle = nullptr;
    InputManager* inputManager = nullptr;
    bool forceClose = false;
};
