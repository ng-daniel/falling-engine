#include "engine/renderer/opengl/opengl_device.h"

void OpenGLDevice::ConfigureWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

/**
 * @brief Initializes OpenGL
 */
bool OpenGLDevice::Init(Window& window) {
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
}

void OpenGLDevice::Submit(UUID mesh) {
}

void OpenGLDevice::Render() {
}

void OpenGLDevice::EndFrame() {
}

void OpenGLDevice::Close() {
}
