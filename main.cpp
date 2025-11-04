#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Camera/camera.h"
#include "World/world.h"
#include "Screen/screen.h"
#include "CollisionSystem/collisionsystem.h"
#include "Material/material.h"
//#include "SpriteEntity/spriteentity.h"
#include "TiledSpriteEntity/tiledspriteentity.h"

#ifdef ENABLEIMGUI
#include "Player/player.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
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
float entSize[2] = {0.0, 0.0};
float entPos[2] = {0.0, 0.0};
int entColor[4] = {0, 0, 0, 0};
bool entStatic = false;
float entScale = 1.0f;
#endif

Material* tileset = nullptr;
Material* playerMat = nullptr;

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

    // Create and load the tileset texture
    tileset = new Material("Assets/sprites/world_tileset.png");
    if (!tileset->Load()) {
        SDL_Log("Failed to load tileset texture!");
        return SDL_APP_FAILURE;
    }

    playerMat = new Material("Assets/sprites/knight.png");
    if (!playerMat->Load()) {
        SDL_Log("Failed to load player texture!");
        return SDL_APP_FAILURE;
    }

    Camera::GetInstance().SetZoom(2.0f);

    Player *player = new Player();
    player->SetScale(1.0f);
    player->SetPosition({0.0, 0.0});
    player->SetColor({255, 0, 0, 255});
    player->SetStatic(true);
    player->SetMaterial(playerMat);
    player->InitializeAnimations();
    player->SetSize({14.0f, 17.0});

    world.AddEntity(player);

    TiledSpriteEntity *wall = new TiledSpriteEntity();
    wall->SetScale(1.0f);
    wall->SetSize({1024.0, 16.0});
    wall->SetPosition({0.0, 20.0});
    wall->SetColor({255, 255, 255, 255});
    wall->SetStatic(true);
    wall->SetMaterial(tileset);
    wall->SetRegion(0.0, 0.0, 16.0, 16.0);

    world.AddEntity(wall);

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
    /* Fucking cast */
    Player *player = reinterpret_cast<Player*>(world.GetLocalPlayer());
    Camera &camera = Camera::GetInstance();
    Vector2 cameraPosition = camera.GetPosition();
    float cameraPos[2] = {cameraPosition.x, cameraPosition.y};
    float cameraScale = camera.GetZoom();

    if(ImGui::Begin("Hello", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Very useful text");
        float scale = player->GetScale();
        if (ImGui::SliderFloat("Player Scale", &scale, 0.0, 10.0, "%.3f", 0))
            player->SetScale(scale);

        Color color = player->GetColor();
        float col[4] = {color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f};
        if (ImGui::ColorEdit4("Color", col)) {
            player->SetColor((Color){(int)(col[0] * 255), (int)(col[1]*255), (int)(col[2]*255), (int)(col[3]*255)});
        }

        if (ImGui::SliderFloat2("Camera Position", cameraPos, -1000.0f, 1000.0f, "%.3f"))
            camera.SetPosition((Vector2){cameraPos[0], cameraPos[1]});

        if (ImGui::SliderFloat("Camera Zoom", &cameraScale, 0.0f, 10.0f, "%.3f"))
            camera.SetZoom(cameraScale);

        ImGui::LabelText("", "Grounded: %i", player->IsOnGround());

        CollisionSystem::Stats stats = collisionSystem.GetGridStats();
        ImGui::BeginGroup();
        ImGui::LabelText("", "Average entity per cell: %.3f", stats.avgEntitiesPerCell);
        ImGui::LabelText("", "Max entities per cell: %i", stats.maxEntitiesPerCell);
        ImGui::LabelText("", "Total cells: %i", stats.totalCells);
        ImGui::LabelText("", "Total entries: %i", stats.totalEntries);
        ImGui::EndGroup();

        if (ImGui::Button("Quit"))
            return SDL_APP_SUCCESS;

    }
    ImGui::End();

    if (ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::SliderFloat2("Position", entPos, -1000, 1000, "%.3f");
            ImGui::SliderFloat2("Size", entSize, 1.0, 1000.0, "%.3f");
            ImGui::SliderInt4("Color", entColor, 0, 255, "%d");
            ImGui::SliderFloat("Scale", &entScale, 0, 10.0, "%.3f");
            
            if (ImGui::Button("Toggle Static")) {
                /* vapo */
                entStatic = entStatic ? false : true;
            }
            ImGui::LabelText("", "Static: %d", entStatic);

            if (ImGui::Button("Add Entity")) {
                    Entity *entity = new Entity();
                    entity->SetScale(entScale);
                    entity->SetSize({entSize[0], entSize[1]});
                    entity->SetPosition({entPos[0], entPos[1]});
                    entity->SetColor({entColor[0], entColor[1], entColor[2], entColor[3]});
                    entity->SetStatic(entStatic);
                    world.AddEntity(entity);
            }

            /* O retangulo do tamanho do trem */
            SDL_FRect rect = {};
            Vector2 pos = Camera::GetInstance().WorldToScreen({entPos[0], entPos[1]});
            rect.w = entSize[0] * Camera::GetInstance().GetZoomOnScreen(entScale);
            rect.h = entSize[1] * Camera::GetInstance().GetZoomOnScreen(entScale);
            rect.x = pos.x - rect.w*0.5f;
            rect.y = pos.y - rect.h*0.5f;

            SDL_SetRenderDrawColor(renderer, entColor[0], entColor[1], entColor[2], entColor[3]);
            SDL_RenderRect(renderer, &rect);
        }
    ImGui::End();
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

    tileset->Unload();
    delete tileset;

    playerMat->Unload();
    delete playerMat;

    for (auto &e : World::GetInstance().GetEntities()) {
        if (e == nullptr)
            continue;
        delete e;
    }
}