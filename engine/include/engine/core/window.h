#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// forward declaration of GLFWwindow struct
struct GLFWwindow;

class Window {
public:
    Window();
    ~Window();

    bool ShouldClose() const;
    void BeginFrame();
    void EndFrame();
private:
    GLFWwindow * handle = nullptr;
};
