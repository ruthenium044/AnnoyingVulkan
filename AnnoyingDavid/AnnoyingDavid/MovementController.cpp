#include "MovementController.h"

bool svk::MovementController::IsDown(SDL_Scancode scancode) {
    return currStates[scancode];
}

bool svk::MovementController::IsUp(SDL_Scancode scancode) {
    return !currStates[scancode];
}

bool svk::MovementController::IsPressed(SDL_Scancode scancode) {
    return currStates[scancode] && !prevStates[scancode];
}

bool svk::MovementController::IsReleased(SDL_Scancode scancode) {
    return !currStates[scancode] && prevStates[scancode];
}

void svk::MovementController::updateKey(SDL_Scancode scancode, bool isDown) {
    currStates[scancode] = isDown;
}

void svk::MovementController::refresh() {
    memcpy(prevStates.data(), currStates.data(), sizeof(currStates));
}

void svk::MovementController::update(float dt, GameObj& gameObject) {
    glm::vec3 rotate{0};
    if (IsDown(keys.lookRight)) rotate.y += 1.f;
    if (IsDown(keys.lookLeft)) rotate.y -= 1.f;
    if (IsDown(keys.lookUp)) rotate.x += 1.f;
    if (IsDown(keys.lookDown)) rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
    }
    
    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};

    glm::vec3 moveDir{0.f};
    if (IsDown(keys.moveForward)) moveDir += forwardDir;
    if (IsDown(keys.moveBackward)) moveDir -= forwardDir;
    if (IsDown(keys.moveRight)) moveDir += rightDir;
    if (IsDown(keys.moveLeft)) moveDir -= rightDir;
    if (IsDown(keys.moveUp)) moveDir += upDir;
    if (IsDown(keys.moveDown)) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
    }
}
