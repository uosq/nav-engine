#pragma once  

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include "../Screen/screen.h"

class Material {
private:
    std::string path;
    unsigned int width = 0;
    unsigned int height = 0;
    SDL_Surface* surface = nullptr;
    SDL_Texture* texture = nullptr;

public:
    Material(const char* path) : path(path) {} 
    ~Material() { Unload(); }

    bool Load() {
        Unload();

        surface = IMG_Load(path.c_str());
        if (!surface) {
            SDL_Log("Failed to load image: %s", path.c_str());
            return false;
        }

        texture = SDL_CreateTextureFromSurface(Screen::GetInstance().GetRenderer(), surface);
        if (!texture) {
            SDL_Log("Failed to create texture from %s : %s", path.c_str(), SDL_GetError());
            return false;
        }

        width = surface->w;
        height = surface->h;

        SDL_DestroySurface(surface);
        surface = nullptr;

        return true;
    }

    void Unload() {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }

        if (surface) {
            SDL_DestroySurface(surface);
            surface = nullptr;
        }
    }

    SDL_Texture* GetTexture() const { return texture; }
    const std::string& GetPath() const { return path; }
    void SetPath(char *path) { this->path = path; }
    unsigned int GetWidth() const { return width; }
    unsigned int GetHeight() const { return height; }

    void Draw(int x, int y, int w = -1, int h = -1) {
        SDL_Renderer *renderer = Screen::GetInstance().GetRenderer();
        if (!texture)
            return;

        SDL_FRect dst;
        dst.x = (float)x;
        dst.y = (float)y;
        dst.w = (w < 0 ? (float)width : (float)w);
        dst.h = (h < 0 ? (float)height : (float)h);

        SDL_RenderTexture(renderer, texture, nullptr, &dst);
    }
};
