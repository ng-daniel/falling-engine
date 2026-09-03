#include "engine/renderer/opengl/opengl_device.h"

#include "glm/glm.hpp"

void OpenGLDevice::ConfigureWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

/**
 * @brief Initializes OpenGL
 */
bool OpenGLDevice::Init(WindowManager& window) {
    this->window = &window;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // failed to initialize GLAD
        this->window->ForceClose();
        return false;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return true;
}

void OpenGLDevice::BeginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void OpenGLDevice::Submit(UUID mesh) {
}

void OpenGLDevice::Render() {
}

void OpenGLDevice::EndFrame() {
}

void OpenGLDevice::Close() {
}
