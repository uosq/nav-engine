#pragma once

#include "../World/world.h"
#include "../CollisionSystem/collisionsystem.h"
#include "../Entity/entity.h"
#include "../Camera/camera.h"
#include <SDL3/SDL_keyboard.h>

#define GRAVITY 980.0f

class Player: public Entity {
private:
    float speed = 200.0f;
    bool isOnGround = false;
    float verticalVelocity = 0.0f;
    const float groundCheckDistance = 1.5f; // Small distance to check below player

public:
    bool IsOnGround() {
        return isOnGround;
    }

    void Process(double dt) override {
        const bool *keys = SDL_GetKeyboardState(nullptr);
        Vector2 moveDir = {0.0f, 0.0f};
        
        // Get input
        if (keys[SDL_SCANCODE_A])
            moveDir.x -= 1.0f;
        if (keys[SDL_SCANCODE_D])
            moveDir.x += 1.0f;
        
        moveDir.Normalize();

        // Apply gravity
        if (!isOnGround) {
            verticalVelocity += GRAVITY * dt;
        } else {
            verticalVelocity = 0.0f;
        }

        // Jump
        if (keys[SDL_SCANCODE_W] && isOnGround) {
            verticalVelocity = -400.0f; // Negative = upward
            isOnGround = false;
        }

        // Calculate desired movement
        Vector2 horizontalMove = {moveDir.x * speed * (float)dt, 0.0f};
        Vector2 verticalMove = {0.0f, verticalVelocity * (float)dt};

        if (horizontalMove.x != 0.0f || horizontalMove.y != 0.0f) {
            Vector2 startPos = position;
            Vector2 endPos = {position.x + horizontalMove.x, position.y + horizontalMove.y};
            
            TraceResult_t hTrace = CollisionSystem::GetInstance().TraceHull(
                startPos, endPos, size, this
            );

            if (hTrace.hit) {
                // Move to the point just before collision
                // Use the hit distance to determine how far we can move
                float safeFraction = std::max(0.0f, hTrace.distance / 
                    std::sqrt(horizontalMove.x * horizontalMove.x + horizontalMove.y * horizontalMove.y) - 0.01f);
                position.x += horizontalMove.x * safeFraction;
            } else {
                // No collision, move freely
                position.x += horizontalMove.x;
            }
        }

        {
            Vector2 startPos = position;
            Vector2 endPos = {position.x, position.y + verticalMove.y};
            
            TraceResult_t trace = CollisionSystem::GetInstance().TraceHull(
                startPos, endPos, size, this
            );

            if (trace.hit) {
                // Hit something
                if (trace.side == CollisionSide::Bottom || trace.side == CollisionSide::Top) {
                    // Vertical collision
                    float safeFraction = std::max(0.0f, trace.distance / std::abs(verticalMove.y) - 0.01f);
                    position.y += verticalMove.y * safeFraction;
                    
                    if (verticalVelocity > 0.0f) {
                        // Hit ground (moving down)
                        isOnGround = true;
                        verticalVelocity = 0.0f;
                    } else {
                        // Hit ceiling (moving up)
                        verticalVelocity = 0.0f;
                    }
                } else {
                    // Side collision during vertical movement (shouldn't happen much)
                    position.y += verticalMove.y;
                }
            } else {
                // No collision, move freely
                position.y += verticalMove.y;
                isOnGround = false;  // In air if no collision below
            }
        }

        {
            Vector2 feetPos = position;
            Vector2 checkEnd = {position.x, position.y + groundCheckDistance};
            
            TraceResult_t groundTrace = CollisionSystem::GetInstance().TraceHull(
                feetPos, checkEnd, size, this
            );

            if (groundTrace.hit && groundTrace.distance <= groundCheckDistance) {
                isOnGround = true;
                // Snap to ground if very close
                if (groundTrace.distance < 1.0f && verticalVelocity >= 0.0f) {
                    position.y = groundTrace.hitPoint.y - (size.y * 0.5f);
                    verticalVelocity = 0.0f;
                }
            } else if (verticalVelocity >= 0.0f) {
                // Only set grounded to false if we're moving down or stationary
                // This prevents losing ground state when moving up
                isOnGround = false;
            }
        }

        // Follow camera
        Camera::GetInstance().SetPosition(position);
    }
};