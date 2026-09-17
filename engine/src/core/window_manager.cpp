#include "engine/core/window_manager.h"

#include "engine/input/input_manager.h"

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const int WIDTH_INIT = 800;
const int HEIGHT_INIT = 600;

WindowManager::~WindowManager() {
    Close();
}

/**
 * @brief Initialize window
 * 
 * @param inputManager Input manager that receives window input events
 * @param configure Function ref supplied by renderer/graphics device
 * for API specific GLFW configuration to run before window creation
 * @return true 
 * @return false 
 */
bool WindowManager::Init(InputManager& inputManager, const std::function<void()>& configure) {
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

    // set callbacks
    this->inputManager = &inputManager;
    glfwSetWindowUserPointer(handle, this);
    glfwSetFramebufferSizeCallback(handle, FrameBufferSizeCallback);
    glfwSetKeyCallback(handle, KeyCallback);
    glfwSetMouseButtonCallback(handle, MouseButtonCallback);
    glfwSetScrollCallback(handle, ScrollCallback);
    glfwSetCursorPosCallback(handle, CursorPositionCallback);
    
    return true;
}

void WindowManager::Close() {
    if (handle) {
        glfwDestroyWindow(handle);
        glfwTerminate();
        handle = nullptr;
    }
    inputManager = nullptr;
}

bool WindowManager::ShouldClose() const {
    return forceClose || (handle ? glfwWindowShouldClose(handle) : true);
}

void WindowManager::ForceClose() {
    forceClose = true;
}

void WindowManager::EndFrame() {
    if (handle) {
        glfwSwapBuffers(handle);
    }
}

void WindowManager::GetFramebufferSize(int& width, int& height) const {
    if (!handle) {
        width = 0;
        height = 0;
        return;
    }
    glfwGetFramebufferSize(handle, &width, &height);
}

void WindowManager::FrameBufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void WindowManager::KeyCallback(GLFWwindow* window, int key, int, int action, int) {
    WindowManager* windowManager = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (windowManager && windowManager->inputManager) {
        windowManager->inputManager->HandleKey(key, action);
    }
}

void WindowManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int) {
    WindowManager* windowManager = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (windowManager && windowManager->inputManager) {
        windowManager->inputManager->HandleMouseButton(button, action);
    }
}

void WindowManager::ScrollCallback(GLFWwindow* window, double, double yOffset) {
    WindowManager* windowManager = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (windowManager && windowManager->inputManager) {
        windowManager->inputManager->HandleScroll(yOffset);
    }
}

void WindowManager::CursorPositionCallback(GLFWwindow* window, double, double) {
    WindowManager* windowManager = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (windowManager && windowManager->inputManager) {
        windowManager->inputManager->HandleCursorPosition();
    }
}
