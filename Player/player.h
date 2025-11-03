#pragma once

#include "../World/world.h"
#include "../CollisionSystem/collisionsystem.h"
#include "../AnimatedEntity/animatedentity.h"
#include "../Camera/camera.h"
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_scancode.h>

#define GRAVITY 980.0f

enum class PlayerAnimationState_t {
    IDLE = 0,
    RUN,
};

class Player : public AnimatedEntity {
private:
    float speed = 200.0f;
    bool isOnGround = false;
    const float groundCheckDistance = 2.0f;
    PlayerAnimationState_t animState;

public:
    bool IsOnGround() {
        return isOnGround;
    }

    void InitializeAnimations() override {
        animations["idle"] = {
            {
                {9,10,13,18},
                {41,10,13,18},
                {73,10,13,18},
                {105,10,13,18},
            },
            0.1f
        };

        animations["walk"] = {
            {
                {8,75,13,17},
                {41,75,13,17},
                {73,75,13,16},
                /*{105,75,13,17},
                {136,75,13,17},
                {169,75,13,17},
                {201,75,13,16},
                {233,75,13,17},
                {8,106,13,18},
                {41,106,13,18},
                {73,106,13,17},
                {105,106,13,18},
                {136,106,13,18},
                {169,106,13,18},
                {201,106,13,17},
                {233,106,13,18},*/
            },
            0.1f
        };
    }

    void Process(double dt) override {
        if (fabs(velocity.x) < 0.5f) {
            animState = PlayerAnimationState_t::IDLE;
        } else {
            animState = PlayerAnimationState_t::RUN;
        }

        UpdateAnimation(dt, animState == PlayerAnimationState_t::IDLE ? "idle" : "walk");

        const bool* keys = SDL_GetKeyboardState(nullptr);
        Vector2 moveDir = {0.0f, 0.0f};

        // Input
        if (keys[SDL_SCANCODE_A]) {
            flipX = true;
            moveDir.x -= 1.0f;
        }
        if (keys[SDL_SCANCODE_D]) {
            flipX = false;
            moveDir.x += 1.0f;
        }

        moveDir.Normalize();

        // Apply gravity
        if (!isOnGround)
            velocity.y += GRAVITY * (float)dt;
        else if (velocity.y > 0.0f)
            velocity.y = 0.0f;

        // Jump
        if (keys[SDL_SCANCODE_W] && isOnGround) {
            velocity.y = -400.0f; // upward impulse
            isOnGround = false;
        }

        // Horizontal control
        const float acceleration = 2000.0f; // how fast to reach max speed
        const float damping = 6.0f;         // how quickly to slow when no input
        const float maxSpeed = speed;

        if (moveDir.x != 0.0f) {
            // Accelerate toward desired direction
            velocity.x += moveDir.x * acceleration * (float)dt;

            // Clamp to max speed
            if (velocity.x >  maxSpeed) velocity.x =  maxSpeed;
            if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;
        } else {
            // No input → apply damping (friction)
            velocity.x -= velocity.x * damping * (float)dt;
        }

        // Predict displacement for this frame
        Vector2 move = velocity * (float)dt;

        // Horizontal movement
        if (move.x != 0.0f) {
            Vector2 startPos = position;
            Vector2 endPos = {position.x + move.x, position.y};

            TraceResult_t trace = CollisionSystem::GetInstance().TraceHull(startPos, endPos, size, this);

            if (trace.hit) {
                float safeMove = std::max(0.0f, trace.distance - 0.01f);
                position.x += (move.x > 0.0f ? 1.0f : -1.0f) * safeMove;
                velocity.x = 0.0f; // stop horizontal motion when hitting wall
            } else {
                position.x += move.x;
            }
        }

        // Vertical movement
        if (move.y != 0.0f) {
            Vector2 startPos = position;
            Vector2 endPos = {position.x, position.y + move.y};

            TraceResult_t trace = CollisionSystem::GetInstance().TraceHull(startPos, endPos, size, this);

            if (trace.hit) {
                float safeMove = std::max(0.0f, trace.distance - 0.01f);
                position.y += (move.y > 0.0f ? 1.0f : -1.0f) * safeMove;

                // Stop vertical velocity depending on direction
                if (move.y > 0.0f) {
                    // Landed on ground
                    isOnGround = true;
                    velocity.y = 0.0f;
                } else {
                    // Hit ceiling
                    velocity.y = 0.0f;
                }
            } else {
                position.y += move.y;
                isOnGround = false;
            }
        }

        // Ground check
        {
            Vector2 feetPos = position;
            Vector2 checkEnd = {position.x, position.y + groundCheckDistance};

            TraceResult_t groundTrace = CollisionSystem::GetInstance().TraceHull(
                feetPos, checkEnd, size, this
            );

            if (groundTrace.hit && groundTrace.distance <= groundCheckDistance) {
                isOnGround = true;
                if (groundTrace.distance < 1.0f && velocity.y >= 0.0f) {
                    position.y = groundTrace.hitPoint.y - (size.y * 0.5f);
                    velocity.y = 0.0f;
                }
            } else if (velocity.y >= 0.0f) {
                isOnGround = false;
            }
        }

        // Follow camera
        Vector2 oldPos = Camera::GetInstance().GetPosition();
        oldPos.Lerp(position, 25.0f * dt);
        Camera::GetInstance().SetPosition(oldPos);
    }
};
