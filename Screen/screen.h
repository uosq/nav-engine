#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "../Vector2/vector2.h"

class Screen {
    private:
    SDL_Window *window;
    SDL_Renderer *renderer;

    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;

    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 800;

    Screen() {
        SDL_SetAppMetadata("Game", "1.0", "com.navet.game");

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
            return;
        }

        float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

        if (!SDL_CreateWindowAndRenderer("Game", (int)(WINDOW_WIDTH * main_scale), (int)(WINDOW_HEIGHT * main_scale), SDL_WINDOW_RESIZABLE, &window, &renderer)) {
            SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
            return;
        }

        SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    };

    public:

    static Screen& GetInstance() {
        static Screen instance;
        return instance;
    };

    SDL_Window* GetWindow() {
        return window;
    };

    SDL_Renderer* GetRenderer() {
        return renderer;
    }

    Vector2 GetSize() {
        return {WINDOW_WIDTH, WINDOW_HEIGHT};
    }
};