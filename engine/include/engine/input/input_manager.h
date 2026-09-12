#pragma once

#include <array>
#include <cstddef>

#include "engine/input/input_structures.h"

struct GLFWwindow;
class WindowManager;

class InputManager {
public:
    InputManager() = default;
    ~InputManager() = default;

    bool Init(WindowManager& windowManager);

    void Update();
    bool IsKeyPressed(KeyCode key) const;
    bool IsKeyDown(KeyCode key) const;
    bool IsKeyReleased(KeyCode key) const;

private:
    struct KeyState {
        bool pressed = false;
        bool down = false;
        bool released = false;
    };

    static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
    static void ScrollCallback(GLFWwindow * window, double xOffset, double yOffset);
    static void CursorPositionCallback(GLFWwindow * window, double xPosition, double yPosition);

    static KeyCode GLFWToKeyCode(int key);
    static KeyCode GLFWToMouseCode(int button);
    static bool IsValidKey(KeyCode key);

    void SetKeyState(KeyCode key, int action);
    void SetKeyState(KeyCode key);

    GLFWwindow* window = nullptr;
    std::array<KeyState, static_cast<std::size_t>(KEY_COUNT)> keyStates{};
};
