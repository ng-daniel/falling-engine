#include "engine/core/window_manager.h"

const int WIDTH_INIT = 800;
const int HEIGHT_INIT = 600;

WindowManager::~WindowManager() {
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
bool WindowManager::Init(const std::function<void()>& configure) {
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
    glfwSwapInterval(1);
    return true;
}

void WindowManager::Close() {
    if (handle) {
        glfwDestroyWindow(handle);
        glfwTerminate();
        handle = nullptr;
    }
}

bool WindowManager::ShouldClose() const {
    return forceClose || (handle ? glfwWindowShouldClose(handle) : true);
}

void WindowManager::ForceClose() {
    forceClose = true;
}

void WindowManager::BeginFrame() {
    if (handle) {
        glfwPollEvents();
    }
}

void WindowManager::EndFrame() {
    if (handle) {
        glfwSwapBuffers(handle);
    }
}
