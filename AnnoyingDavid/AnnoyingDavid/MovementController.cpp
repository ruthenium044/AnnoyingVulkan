#include "MovementController.h"

bool svk::MovementController::isDown(SDL_Scancode scancode) {
    return currStates[scancode];
}

bool svk::MovementController::isUp(SDL_Scancode scancode) {
    return !currStates[scancode];
}

bool svk::MovementController::isPressed(SDL_Scancode scancode) {
    return currStates[scancode] && !prevStates[scancode];
}

bool svk::MovementController::isReleased(SDL_Scancode scancode) {
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
    if (isDown(keys.lookRight)) rotate.y += 1.f;
    if (isDown(keys.lookLeft)) rotate.y -= 1.f;
    if (isDown(keys.lookUp)) rotate.x += 1.f;
    if (isDown(keys.lookDown)) rotate.x -= 1.f;

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
    if (isDown(keys.moveForward)) moveDir += forwardDir;
    if (isDown(keys.moveBackward)) moveDir -= forwardDir;
    if (isDown(keys.moveRight)) moveDir += rightDir;
    if (isDown(keys.moveLeft)) moveDir -= rightDir;
    if (isDown(keys.moveUp)) moveDir += upDir;
    if (isDown(keys.moveDown)) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
    }
}
