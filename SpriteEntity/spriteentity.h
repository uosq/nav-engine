#pragma once

#include "../Entity/entity.h"
#include <SDL3/SDL_render.h>

class SpriteEntity: public Entity {    
    protected:
    bool flipX = false, flipY = false;
    Material* material = nullptr;

    // Spritesheet region
    SDL_FRect srcRect = {0, 0, 0, 0};

    public:
    void SetMaterial(Material* m) { material = m; }
    Material* GetMaterial() const { return material; }

    // Spritesheet region
    void SetRegion(float x, float y, float w, float h) {
        srcRect.x = x;
        srcRect.y = y;
        srcRect.w = w;
        srcRect.h = h;
    }

    void Draw() override {
        if (!material || !material->GetTexture()) {
            SDL_Log("SpriteEntity: Couldn't find material/texture!");
            return;
        }

        SDL_Renderer *renderer = Screen::GetInstance().GetRenderer();
        Camera& camera = Camera::GetInstance();
        Vector2 screenPos = camera.WorldToScreen(position);

        SDL_FRect rect;
        rect.w = (flipX ? -1.0f : 1.0f) * srcRect.w * camera.GetZoomOnScreen(scale);
        rect.h = (flipY ? -1.0f : 1.0f) * srcRect.h * camera.GetZoomOnScreen(scale);
        rect.x = screenPos.x - (rect.w * 0.5f);
        rect.y = screenPos.y - (rect.h * 0.5f);

        /* debug */
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
        /**/

        // Use srcRect only if useRegion is true, otherwise use entire texture
        SDL_RenderTexture(renderer, material->GetTexture(), &srcRect, &rect);
    }
};