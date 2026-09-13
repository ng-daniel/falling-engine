#include "engine/input/input_manager.h"

#include <GLFW/glfw3.h>

void InputManager::Update() {
    // reset press and release frame events
    for (KeyState& state : keyStates) {
        state.pressed = false;
        state.released = false;
    }

    // reset mouse scroll and move events
    keyStates[KEY_MOUSE_SCROLL_UP].down = false;
    keyStates[KEY_MOUSE_SCROLL_DOWN].down = false;
    keyStates[KEY_MOUSE_MOVE].down = false;

    glfwPollEvents();
}

/**
 * @brief Check if the specified key was pressed in the current frame
 * 
 * @param key 
 * @return true 
 * @return false 
 */
bool InputManager::IsKeyPressed(KeyCode key) const {
    return IsValidKey(key) && keyStates[static_cast<std::size_t>(key)].pressed;
}

/**
 * @brief Check if the specified key is currently being held down,
 * regardless of whether it was pressed in the current frame
 * 
 * @param key 
 * @return true 
 * @return false 
 */
bool InputManager::IsKeyDown(KeyCode key) const {
    return IsValidKey(key) && keyStates[static_cast<std::size_t>(key)].down;
}

/**
 * @brief Check if the specified key was released in the current frame
 * 
 * @param key 
 * @return true 
 * @return false 
 */
bool InputManager::IsKeyReleased(KeyCode key) const {
    return IsValidKey(key) && keyStates[static_cast<std::size_t>(key)].released;
}

void InputManager::HandleKey(int key, int action) {
    SetKeyState(GLFWToKeyCode(key), action);
}

void InputManager::HandleMouseButton(int button, int action) {
    SetKeyState(GLFWToMouseCode(button), action);
}

void InputManager::HandleScroll(double yOffset) {
    if (yOffset > 0.0) {
        SetKeyState(KEY_MOUSE_SCROLL_UP);
    } else if (yOffset < 0.0) {
        SetKeyState(KEY_MOUSE_SCROLL_DOWN);
    }
}

void InputManager::HandleCursorPosition() {
    SetKeyState(KEY_MOUSE_MOVE);
}


/**
 * @brief Translation bit to map GLFW key codes to public KeyCode enums
 * 
 * @param key 
 * @return KeyCode 
 */
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

/**
 * @brief Map GLFW mouse button codes to public KeyCode enums
 * 
 * @param button 
 * @return KeyCode 
 */
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

/**
 * @brief Map GLFW key actions to internal keystates
 * 
 * @param key 
 * @param action 
 */
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
