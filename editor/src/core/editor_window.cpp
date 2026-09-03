#include "editor/core/editor_window.h"
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

EditorWindow::EditorWindow() {

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

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 6.0f;            // Rounding of window edges
    style.FrameRounding = 4.0f;             // Rounding of buttons, checkboxes, input boxes
    style.ScrollbarRounding = 12.0f;        // Rounding of scrollbars
    style.FramePadding = ImVec2(6, 4); // Internal padding for individual widgets

    // set accented colors for all the UI to red, including buttons, sliders, etc.
    ImVec4 accentColor = ImVec4(0.8f, 0.2f, 0.1f, 1.0f); // Red color
    float hoverFactor = 1.2f;
    float activeFactor = 1.5f;
    float darkenedFactor = 0.2f;
    
    ImVec4 accentColorHovered = ImVec4(
        accentColor.x * hoverFactor,
        accentColor.y * hoverFactor,
        accentColor.z * hoverFactor,
        accentColor.w);

    ImVec4 accentColorActive = ImVec4(
        accentColor.x * activeFactor,
        accentColor.y * activeFactor,
        accentColor.z * activeFactor,
        accentColor.w);
    ImVec4 accentColorDarkened = ImVec4(
        accentColor.x * darkenedFactor,
        accentColor.y * darkenedFactor,
        accentColor.z * darkenedFactor,
        accentColor.w);

    style.Colors[ImGuiCol_TitleBg] = accentColor;
    style.Colors[ImGuiCol_TitleBgActive] = accentColorActive;
    style.Colors[ImGuiCol_TitleBgCollapsed] = accentColor;

    style.Colors[ImGuiCol_Header] = accentColor;
    style.Colors[ImGuiCol_HeaderHovered] = accentColorHovered;
    style.Colors[ImGuiCol_HeaderActive] = accentColorActive;

    style.Colors[ImGuiCol_FrameBg] = accentColorDarkened;

    style.Colors[ImGuiCol_Button] = accentColor;
    style.Colors[ImGuiCol_ButtonHovered] = accentColorHovered;
    style.Colors[ImGuiCol_ButtonActive] = accentColorActive;

    style.Colors[ImGuiCol_SliderGrab] = accentColor;
    style.Colors[ImGuiCol_SliderGrabActive] = accentColorActive;


    ImGui_ImplGlfw_InitForOpenGL(handle, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);
}

EditorWindow::~EditorWindow() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (handle) {
        glfwDestroyWindow(handle);
    }
    glfwTerminate();
}

bool EditorWindow::ShouldClose() const {
    return !handle || glfwWindowShouldClose(handle);
}

void EditorWindow::BeginFrame() {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EditorWindow::EndFrame() {
    ImGui::Render();

    int width, height;
    glfwGetFramebufferSize(handle, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(handle);
}