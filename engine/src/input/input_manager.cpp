#include "engine/input/input_manager.h"

#include "engine/core/window_manager.h"

#include <GLFW/glfw3.h>

bool InputManager::Init(WindowManager& windowManager) {
    window = windowManager.handle;
    if (!window) {
        return false;
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    return true;
}

void InputManager::Update() {
    for (KeyState& state : keyStates) {
        state.pressed = false;
        state.released = false;
    }

    keyStates[KEY_MOUSE_SCROLL_UP].down = false;
    keyStates[KEY_MOUSE_SCROLL_DOWN].down = false;
    keyStates[KEY_MOUSE_MOVE].down = false;

    if (window) {
        glfwPollEvents();
    }
}

bool InputManager::IsKeyPressed(KeyCode key) const {
    return IsValidKey(key) && keyStates[static_cast<std::size_t>(key)].pressed;
}

bool InputManager::IsKeyDown(KeyCode key) const {
    return IsValidKey(key) && keyStates[static_cast<std::size_t>(key)].down;
}

bool InputManager::IsKeyReleased(KeyCode key) const {
    return IsValidKey(key) && keyStates[static_cast<std::size_t>(key)].released;
}

void InputManager::KeyCallback(GLFWwindow* window, int key, int, int action, int) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (inputManager) {
        inputManager->SetKeyState(GLFWToKeyCode(key), action);
    }
}

void InputManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (inputManager) {
        inputManager->SetKeyState(GLFWToMouseCode(button), action);
    }
}

void InputManager::ScrollCallback(GLFWwindow* window, double, double yOffset) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!inputManager) {
        return;
    }

    if (yOffset > 0.0) {
        inputManager->SetKeyState(KEY_MOUSE_SCROLL_UP);
    } else if (yOffset < 0.0) {
        inputManager->SetKeyState(KEY_MOUSE_SCROLL_DOWN);
    }
}

void InputManager::CursorPositionCallback(GLFWwindow* window, double, double) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (inputManager) {
        inputManager->SetKeyState(KEY_MOUSE_MOVE);
    }
}

KeyCode InputManager::GLFWToKeyCode(int key) {
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
        return static_cast<KeyCode>(KEY_A + key - GLFW_KEY_A);
    }
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
        return static_cast<KeyCode>(KEY_0 + key - GLFW_KEY_0);
    }

    switch (key) {
        case GLFW_KEY_ESCAPE: return KEY_ESCAPE;
        case GLFW_KEY_ENTER: return KEY_ENTER;
        case GLFW_KEY_TAB: return KEY_TAB;
        case GLFW_KEY_BACKSPACE: return KEY_BACKSPACE;
        case GLFW_KEY_INSERT: return KEY_INSERT;
        case GLFW_KEY_DELETE: return KEY_DELETE;
        case GLFW_KEY_RIGHT: return KEY_ARROW_RIGHT;
        case GLFW_KEY_LEFT: return KEY_ARROW_LEFT;
        case GLFW_KEY_DOWN: return KEY_ARROW_DOWN;
        case GLFW_KEY_UP: return KEY_ARROW_UP;
        case GLFW_KEY_F1: return KEY_F1;
        case GLFW_KEY_F2: return KEY_F2;
        case GLFW_KEY_F3: return KEY_F3;
        case GLFW_KEY_F4: return KEY_F4;
        case GLFW_KEY_F5: return KEY_F5;
        case GLFW_KEY_F6: return KEY_F6;
        case GLFW_KEY_F7: return KEY_F7;
        case GLFW_KEY_F8: return KEY_F8;
        case GLFW_KEY_F9: return KEY_F9;
        case GLFW_KEY_F10: return KEY_F10;
        case GLFW_KEY_F11: return KEY_F11;
        case GLFW_KEY_F12: return KEY_F12;
        default: return KEY_UNKNOWN;
    }
}

KeyCode InputManager::GLFWToMouseCode(int button) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: return KEY_MOUSE_LMB;
        case GLFW_MOUSE_BUTTON_RIGHT: return KEY_MOUSE_RMB;
        default: return KEY_UNKNOWN;
    }
}

bool InputManager::IsValidKey(KeyCode key) {
    return key > KEY_UNKNOWN && key < KEY_COUNT;
}

void InputManager::SetKeyState(KeyCode key, int action) {
    if (!IsValidKey(key)) {
        return;
    }

    KeyState& state = keyStates[static_cast<std::size_t>(key)];
    if (action == GLFW_PRESS) {
        if (!state.down) {
            state.pressed = true;
        }
        state.down = true;
    } else if (action == GLFW_RELEASE) {
        if (state.down) {
            state.released = true;
        }
        state.down = false;
    } else if (action == GLFW_REPEAT) {
        state.down = true;
    }
}

void InputManager::SetKeyState(KeyCode key) {
    if (!IsValidKey(key)) {
        return;
    }

    KeyState& state = keyStates[static_cast<std::size_t>(key)];
    state.pressed = true;
    state.down = true;
}
