#pragma once

#include "../SpriteEntity/spriteentity.h"

class TiledSpriteEntity: public SpriteEntity {
    public:
    void Draw() override {
        if (!material || !material->GetTexture()) {
            SDL_Log("RepeatedSpriteEntity: Couldn't find material/texture!");
            return;
        }

        SDL_Renderer *renderer = Screen::GetInstance().GetRenderer();
        Camera& camera = Camera::GetInstance();
        Vector2 screenPos = camera.WorldToScreen(position);

        SDL_FRect rect;
        rect.w = (flipX ? -1.0f : 1.0f) * size.x * camera.GetZoomOnScreen(scale);
        rect.h = (flipY ? -1.0f : 1.0f) * size.y * camera.GetZoomOnScreen(scale);
        rect.x = screenPos.x - (rect.w * 0.5f);
        rect.y = screenPos.y - (rect.h * 0.5f);

        SDL_RenderTextureTiled(renderer, material->GetTexture(), &srcRect, camera.GetZoom(), &rect);

        // compute tile size to fill exactly tileCountX and tileCountY
        /*float tileW = rect.w / tileCountX;
        float tileH = rect.h / tileCountY;
        SDL_FRect dst;
        dst.w = tileW;
        dst.h = tileH;
        for (int y = 0; y < tileCountY; ++y) {
            for (int x = 0; x < tileCountX; ++x) {
                dst.x = rect.x + x * tileW;
                dst.y = rect.y + y * tileH;
                SDL_RenderTexture(renderer, material->GetTexture(), &srcRect, &dst);
            }
        }*/
    }
};