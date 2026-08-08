#include "editor/core/window.h"
#include "engine/debug/logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace {
    const int INITIAL_WINDOW_WIDTH = 1280;
    const int INITIAL_WINDOW_HEIGHT = 800;

    const char * WINDOW_TITLE = "Falling Engine Editor";
    const char * GLSL_VERSION = "#version 330";
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

    handle = glfwCreateWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!handle) {
        glfwTerminate();
        Logger::Error("Window", "Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(handle);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::Error("Window", "Failed to initialize GLAD");
        return;
    }

    /// Initialize ImGui
    /// ---------------------------------------------

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(handle, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);
}

Window::~Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (handle) {
        glfwDestroyWindow(handle);
    }
    glfwTerminate();
}

bool Window::ShouldClose() const {
    return !handle || glfwWindowShouldClose(handle);
}

void Window::BeginFrame() {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::EndFrame() {
    ImGui::Render();

    int width, height;
    glfwGetFramebufferSize(handle, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(handle);
}