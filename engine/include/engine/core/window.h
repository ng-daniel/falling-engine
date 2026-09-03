#pragma once

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>

class Window {
public:
    ~Window();
    bool Init(const std::function<void()>& configure = {});
    bool ShouldClose() const;
    void ForceClose();
    void BeginFrame();
    void EndFrame();
    void Close();
private:
    GLFWwindow * handle = nullptr;
    bool forceClose = false;
};
