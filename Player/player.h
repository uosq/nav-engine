#pragma once

#include "../World/world.h"
#include "../CollisionSystem/collisionsystem.h"
#include "../Entity/entity.h"
#include "../Camera/camera.h"
#include <SDL3/SDL_keyboard.h>

#define GRAVITY 100.0f

class Player: public Entity {
    private:
    float speed = 100.0f;
    bool grounded = false;
    float verticalVelocity = 0.0;

    public:
    bool IsGrounded() {
        return grounded;
    }

    void Process(double dt) override {
        const bool *keys = SDL_GetKeyboardState(nullptr);
        Vector2 dir = {0.0, 0.0};
        
        if (keys[SDL_SCANCODE_A])
            dir.x -= 1.0;
        if (keys[SDL_SCANCODE_D])
            dir.x += 1.0;
        dir.Normalize();
        
        // Horizontal movement
        position.x += dir.x * speed * dt;
        
        // Apply gravity
        if (!grounded) {
            verticalVelocity += GRAVITY * dt;
        }
        
        // Jump
        if (keys[SDL_SCANCODE_W] && grounded) {
            verticalVelocity = -200.0f; // Negative = upward
            grounded = false;
        }
        
        // Vertical movement
        position.y += verticalVelocity * dt;
        
        Vector2 feetPos = {position.x, position.y + size.y * 0.5f}; // Bottom of player
        Vector2 checkEnd = {feetPos.x, feetPos.y + 0.1f};
        
        TraceResult_t trace = CollisionSystem::GetInstance().TraceLine(
            feetPos, checkEnd, this
        );
        
        if (trace.hit) {
            grounded = true;
            verticalVelocity = 0.0f; // Stop falling
        } else {
            grounded = false;
        }
        
        // Follow camera
        Camera::GetInstance().SetPosition(position);
    }

};