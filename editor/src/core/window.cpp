#include "editor/core/window.h"
#include "engine/debug/logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace {
    const int INITIAL_WINDOW_WIDTH = 800;
    const int INITIAL_WINDOW_HEIGHT = 600;

    const char * WINDOW_TITLE = "Falling Engine Editor";
}

Window::Window() {
    
    /// Initialize GLFW
    /// ---------------------------------------------

    if (!glfwInit()) {
        Logger::Error("Window", "Failed to initialize GLFW");
        return;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) {
        glfwTerminate();
        Logger::Error("Window", "Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::Error("Window", "Failed to initialize GLAD");
        return;
    }

    /// Main loop
    /// ---------------------------------------------

    while (!glfwWindowShouldClose(window)) {

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /// Cleanup
    /// ---------------------------------------------

    glfwDestroyWindow(window);
    glfwTerminate();
}