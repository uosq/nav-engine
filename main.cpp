#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include "Material/materialmanager.h"
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Camera/camera.h"
#include "World/world.h"
#include "Screen/screen.h"
#include "CollisionSystem/collisionsystem.h"
//#include "SpriteEntity/spriteentity.h"
#include "Material/materialmanager.h"

//#define ENABLEIMGUI

#ifdef ENABLEIMGUI
#include "Editor/editor.h"
#include "TiledSpriteEntity/tiledspriteentity.h"
#include "Player/player.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

static WorldEditor editor(World::GetInstance());
#endif

#ifdef ENABLELUA
extern "C" {
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
}

#include <lua5.4/lua.hpp>
#include "Lua/Classes/vector2.h"
#include "Lua/Libraries/camera.h"
#endif

double oldNow = 0.0;
#ifdef ENABLEIMGUI
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
float entSize[2] = {500.0, 500.0};
float entPos[2] = {0.0, 0.0};
float entColor[4] = {1.0, 1.0, 1.0, 1.0};
bool entStatic = false;
float entScale = 1.0f;
#endif

#ifdef ENABLELUA
lua_State *L = nullptr;
#endif

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    #ifdef ENABLELUA
    L = luaL_newstate();
    luaL_openlibs(L);

    // Lua classes
    Register_Vector2Class(L);

    // Lua libraries
    luaopen_camera(L);
    #endif

    /* Initialize our world */
    Screen &screen = Screen::GetInstance();
    World &world = World::GetInstance();
    Camera::GetInstance();

    #ifdef ENABLEIMGUI
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(screen.GetWindow(), screen.GetRenderer());
    ImGui_ImplSDLRenderer3_Init(screen.GetRenderer());
    #endif

    SDL_Log("%s", SDL_GetBasePath());

    MaterialManager::GetInstance().LoadMaterial("player", "Assets/sprites/knight.png");
    MaterialManager::GetInstance().LoadMaterial("tiles", "Assets/sprites/world_tileset.png");

    Camera::GetInstance().SetZoom(2.0f);

    /*Player *player = new Player();
    player->SetScale(1.0f);
    player->SetPosition({0.0, 0.0});
    player->SetColor({255, 0, 0, 255});
    player->SetStatic(true);
    player->SetMaterial(playerMat);
    player->InitializeAnimations();
    player->SetSize({14.0f, 17.0});

    world.SetLocalPlayer(player);
    world.AddEntity(player);

    TiledSpriteEntity *wall = new TiledSpriteEntity();
    wall->SetScale(1.0f);
    wall->SetSize({1024.0, 16.0});
    wall->SetPosition({0.0, 20.0});
    wall->SetColor({255, 255, 255, 255});
    wall->SetStatic(true);
    wall->SetMaterial(tileset);
    wall->SetRegion(0.0, 0.0, 16.0, 16.0);

    world.AddEntity(wall);*/

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
            return SDL_APP_SUCCESS;
        }
    }

    #ifdef ENABLEIMGUI
    ImGui_ImplSDL3_ProcessEvent(event);
    #endif

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    const double now = ((double)SDL_GetTicks()) / 1000.0; // current time in seconds
    double deltaTime = now - oldNow; // time since last frame
    oldNow = now; // update for next iteration

    Screen &screen = Screen::GetInstance();
    World &world = World::GetInstance();
    auto &collisionSystem = CollisionSystem::GetInstance();
    SDL_Renderer *renderer = screen.GetRenderer();
    Camera &camera = Camera::GetInstance();

    world.ProcessEntities(deltaTime);

    auto collisions = collisionSystem.DetectCollisions(world.GetEntities());
    collisionSystem.ResolveCollisions(collisions);

    #ifdef ENABLEIMGUI
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    #endif

    SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0.0, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(renderer);

    #ifdef ENABLEIMGUI
        // Update editor
        editor.Update(deltaTime, renderer);

        // Draw editor UI
        editor.DrawUI();
    #endif

    world.DrawEntities();

    #ifdef ENABLEIMGUI
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    #endif

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    #ifdef ENABLEIMGUI
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    #endif

    #ifdef ENABLELUA
    lua_close(L);
    #endif

    MaterialManager::GetInstance().UnloadAll();

    for (auto &e : World::GetInstance().GetEntities()) {
        if (e == nullptr)
            continue;
        delete e;
    }
}