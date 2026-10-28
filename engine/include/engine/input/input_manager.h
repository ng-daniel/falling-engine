#pragma once

#include <array>
#include <cstddef>

#include "engine/input/input_structures.h"
#include "engine/utils/vector.h"

class InputManager {
public:
    InputManager() = default;
    ~InputManager() = default;

    void Update();
    bool IsKeyPressed(KeyCode key) const;
    bool IsKeyDown(KeyCode key) const;
    bool IsKeyReleased(KeyCode key) const;
    Vector2 GetCursorPosition() const;
    Vector2 GetCursorDelta() const;

    void HandleKey(int key, int action);
    void HandleMouseButton(int button, int action);
    void HandleScroll(double yOffset);
    void HandleCursorPosition(double xPosition, double yPosition);

private:
    struct KeyState {
        bool pressed = false;
        bool down = false;
        bool released = false;
    };

    static KeyCode GLFWToKeyCode(int key);
    static KeyCode GLFWToMouseCode(int button);
    static bool IsValidKey(KeyCode key);

    void SetKeyState(KeyCode key, int action);
    void SetKeyState(KeyCode key);

    std::array<KeyState, static_cast<std::size_t>(KEY_COUNT)> keyStates{};

    Vector2 cursorPosition;
    Vector2 cursorDelta;
    bool hasCursorSample = false;
};
