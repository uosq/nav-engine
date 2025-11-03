#pragma once

#include "../Vector2/vector2.h"
#include "../Color/color.h"
#include "../Camera/camera.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

class Entity {
    protected:
    Vector2 position = {0.0, 0.0};
    Vector2 velocity = {0.0, 0.0};
    Vector2 size = {0.0, 0.0};
    float scale = 1.0f;
    Color color = {0, 0, 0, 255};
    bool isStatic = false;

    public:

    Vector2 GetPosition() { return this->position; }
    void SetPosition(Vector2 position) { this->position = position; };
    
    Vector2 GetSize() { return this->size; }
    void SetSize(Vector2 size) { this->size = size; }
    
    float GetScale() { return this->scale; }
    void SetScale(float scale) { this->scale = scale; }
    
    bool IsStatic() { return this->isStatic; }
    void SetStatic(bool isStatic) { this->isStatic = isStatic; }

    void SetColor(Color color) { this->color = color; }
    Color GetColor() { return this->color; }

    virtual void Process(double dt) { }

    virtual void Draw(SDL_Renderer *renderer) {
        SDL_FRect rect = {};
        Camera &camera = Camera::GetInstance();
        Vector2 screenPos = camera.WorldToScreen(position);
        rect.w = size.x * camera.GetZoomOnScreen(scale);
        rect.h = size.y * camera.GetZoomOnScreen(scale);
        rect.x = screenPos.x - (rect.w * 0.5f);
        rect.y = screenPos.y - (rect.h * 0.5f);

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    Entity() {
        position = {0.0, 0.0};
        size = {0.0, 0.0};
        scale = 1.0f;
        color = {0, 0, 0, 255};
        isStatic = true;
    }
};