#include "engine/renderer/debug_camera.h"

#include "engine/core/window_manager.h"
#include "engine/input/input_manager.h"

#include <algorithm>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

namespace {
    constexpr float FIELD_OF_VIEW_DEGREES = 60.0f;
    constexpr float NEAR_PLANE = 0.1f;
    constexpr float FAR_PLANE = 100.0f;

    constexpr float MOVE_SPEED = 3.0f;
    constexpr float ROTATION_SPEED_DEGREES = 90.0f;
    constexpr float MAX_FRAME_TIME = 0.1f;
    constexpr float MAX_PITCH_DEGREES = 89.0f;
    constexpr glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);

    glm::vec3 CameraForward(float yawDegrees, float pitchDegrees) {
        const float yaw = glm::radians(yawDegrees);
        const float pitch = glm::radians(pitchDegrees);
        return glm::normalize(glm::vec3(
            std::cos(yaw) * std::cos(pitch),
            std::sin(pitch),
            std::sin(yaw) * std::cos(pitch)
        ));
    }
}

FrameUniformData DebugCamera::Update(const WindowManager& window, const InputManager& inputManager) {
    const auto now = std::chrono::steady_clock::now();
    const float deltaSeconds = std::min(
        std::chrono::duration<float>(now - lastUpdateTime).count(),
        MAX_FRAME_TIME
    );
    lastUpdateTime = now;

    if (inputManager.IsKeyDown(KEY_ARROW_LEFT)) {
        yawDegrees -= ROTATION_SPEED_DEGREES * deltaSeconds;
    }
    if (inputManager.IsKeyDown(KEY_ARROW_RIGHT)) {
        yawDegrees += ROTATION_SPEED_DEGREES * deltaSeconds;
    }
    if (inputManager.IsKeyDown(KEY_ARROW_UP)) {
        pitchDegrees += ROTATION_SPEED_DEGREES * deltaSeconds;
    }
    if (inputManager.IsKeyDown(KEY_ARROW_DOWN)) {
        pitchDegrees -= ROTATION_SPEED_DEGREES * deltaSeconds;
    }
    pitchDegrees = std::clamp(pitchDegrees, -MAX_PITCH_DEGREES, MAX_PITCH_DEGREES);

    const glm::vec3 forward = CameraForward(yawDegrees, pitchDegrees);
    const glm::vec3 horizontalForward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
    const glm::vec3 right = glm::normalize(glm::cross(horizontalForward, WORLD_UP));
    const float movement = MOVE_SPEED * deltaSeconds;

    if (inputManager.IsKeyDown(KEY_W)) {
        position += horizontalForward * movement;
    }
    if (inputManager.IsKeyDown(KEY_S)) {
        position -= horizontalForward * movement;
    }
    if (inputManager.IsKeyDown(KEY_A)) {
        position -= right * movement;
    }
    if (inputManager.IsKeyDown(KEY_D)) {
        position += right * movement;
    }
    if (inputManager.IsKeyDown(KEY_Q)) {
        position += WORLD_UP * movement;
    }
    if (inputManager.IsKeyDown(KEY_E)) {
        position -= WORLD_UP * movement;
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    window.GetFramebufferSize(framebufferWidth, framebufferHeight);
    if (framebufferWidth > 0 && framebufferHeight > 0) {
        aspectRatio = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
    }

    FrameUniformData frame;
    frame.view = Matrix4(glm::lookAt(position, position + forward, WORLD_UP));
    frame.projection = Matrix4(glm::perspective(
        glm::radians(FIELD_OF_VIEW_DEGREES),
        aspectRatio,
        NEAR_PLANE,
        FAR_PLANE
    ));
    return frame;
}
