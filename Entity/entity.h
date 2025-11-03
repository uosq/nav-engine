#pragma once

#include "../Vector2/vector2.h"
#include "../Color/color.h"
#include "../Camera/camera.h"
#include "../Material/material.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

class Entity {
protected:
    Vector2 position = {0.0, 0.0};
    Vector2 velocity = {0.0, 0.0};
    Vector2 size = {0.0, 0.0};
    float scale = 1.0f;
    Color color = {255, 255, 255, 255};
    bool isStatic = false;

public:
    Entity() = default;
    virtual ~Entity() = default;

    // Basic properties
    Vector2 GetPosition() const { return position; }
    void SetPosition(Vector2 pos) { position = pos; }

    Vector2 GetSize() const { return size; }
    void SetSize(Vector2 s) { size = s; }

    float GetScale() const { return scale; }
    void SetScale(float s) { scale = s; }

    bool IsStatic() const { return isStatic; }
    void SetStatic(bool s) { isStatic = s; }

    Color GetColor() const { return color; }
    void SetColor(Color c) { color = c; }

    Vector2 GetVelocity() const { return velocity; }
    void SetVelocity(Vector2 v) { velocity = v; }

    virtual void Process(double dt) { }

    virtual void Draw() {
        SDL_Renderer *renderer = Screen::GetInstance().GetRenderer();
        Camera& camera = Camera::GetInstance();
        Vector2 screenPos = camera.WorldToScreen(position);

        SDL_FRect rect;
        rect.w = size.x * camera.GetZoomOnScreen(scale);
        rect.h = size.y * camera.GetZoomOnScreen(scale);
        rect.x = screenPos.x - (rect.w * 0.5f);
        rect.y = screenPos.y - (rect.h * 0.5f);

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }

};
