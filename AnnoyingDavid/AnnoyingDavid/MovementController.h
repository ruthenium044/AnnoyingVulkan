#pragma once

#include <array>

#include "GameObj.h"

namespace svk {
    class MovementController {
    public:
        struct KeyMappings {
            SDL_Scancode moveLeft = SDL_SCANCODE_A;
            SDL_Scancode moveRight = SDL_SCANCODE_D;
            SDL_Scancode moveForward = SDL_SCANCODE_W;
            SDL_Scancode moveBackward = SDL_SCANCODE_S;
            SDL_Scancode moveUp = SDL_SCANCODE_E;
            SDL_Scancode moveDown = SDL_SCANCODE_Q;
            SDL_Scancode lookLeft = SDL_SCANCODE_LEFT;
            SDL_Scancode lookRight = SDL_SCANCODE_RIGHT;
            SDL_Scancode lookUp = SDL_SCANCODE_UP;
            SDL_Scancode lookDown = SDL_SCANCODE_DOWN;
        };

        std::array<int, SDL_NUM_SCANCODES> prevStates;
        std::array<int, SDL_NUM_SCANCODES> currStates;

        bool isDown(SDL_Scancode scancode);
        bool isUp(SDL_Scancode scancode);
        bool isPressed(SDL_Scancode scancode);
        bool isReleased(SDL_Scancode scancode);

        void updateKey(SDL_Scancode scancode, bool isDown);
        void refresh();
        void update(float dt, GameObj& gameObj);

        KeyMappings keys{};
        float moveSpeed{3.f};
        float lookSpeed{1.5f};
    };
}
