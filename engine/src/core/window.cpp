#include "engine/core/window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const int WIDTH_INIT = 800;
const int HEIGHT_INIT = 600;

Window::Window() {
    if (!glfwInit()) {
        // Initialization failed
        handle = nullptr;
        return;
    }

    handle = glfwCreateWindow(WIDTH_INIT, HEIGHT_INIT, "Falling Engine", nullptr, nullptr);
    if (!handle) {
        // Window or OpenGL context creation failed
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(handle);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // Failed to initialize GLAD
        glfwDestroyWindow(handle);
        glfwTerminate();
        handle = nullptr;
        return;
    }
}

Window::~Window() {
    if (handle) {
        glfwDestroyWindow(handle);
        glfwTerminate();
        handle = nullptr;
    }
}

bool Window::ShouldClose() const {
    return handle ? glfwWindowShouldClose(handle) : true;
}

void Window::BeginFrame() {
    if (handle) {
        glfwPollEvents();
    }
}

void Window::EndFrame() {
    if (handle) {
        glfwSwapBuffers(handle);
    }
}