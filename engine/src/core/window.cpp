#include "engine/core/window.h"

const int WIDTH_INIT = 800;
const int HEIGHT_INIT = 600;

Window::~Window() {
    Close();
}

/**
 * @brief Initialize window
 * 
 * @param configure Function ref supplied by renderer/graphics device 
 * for API specific GLFW configuration to run before window creation
 * @return true 
 * @return false 
 */
bool Window::Init(const std::function<void()>& configure) {
    if (!glfwInit()) {
        // Initialization failed
        handle = nullptr;
        return false;
    }
    if (configure) {
        configure();
    }
    handle = glfwCreateWindow(WIDTH_INIT, HEIGHT_INIT, "Falling Engine", nullptr, nullptr);
    if (!handle) {
        // Window or OpenGL context creation failed
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(handle);
    return true;
}

void Window::Close() {
    if (handle) {
        glfwDestroyWindow(handle);
        glfwTerminate();
        handle = nullptr;
    }
}

bool Window::ShouldClose() const {
    return forceClose || (handle ? glfwWindowShouldClose(handle) : true);
}

void Window::ForceClose() {
    forceClose = true;
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
