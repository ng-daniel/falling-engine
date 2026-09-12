#pragma once

#include <functional>

struct GLFWwindow;
class InputManager;

class WindowManager {
public:
    ~WindowManager();

    bool Init(const std::function<void()>& configure = {});
    bool ShouldClose() const;
    void ForceClose();
    void EndFrame();
    void Close();

    void GetFramebufferSize(int& width, int& height) const;
private:
    friend class InputManager;

    GLFWwindow * handle = nullptr;
    bool forceClose = false;
};
