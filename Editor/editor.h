#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "../EntityTemplate/entitytemplate.h"

#include "../imgui.h"
#include "../imgui_impl_sdl3.h"
#include "../imgui_impl_sdlrenderer3.h"

static Vector2 GetMousePosition() {
    float mousePosX, mousePosY;
    SDL_GetMouseState(&mousePosX, &mousePosY);
    return {mousePosX, mousePosY};
}

class WorldEditor {
private:
    World& world;
    
    // Editor state
    std::vector<EntityTemplate> templates;
    int selectedTemplate = 0;
    EntityTemplate currentTemplate;
    
    // UI state
    float entPos[2] = {0, 0};
    float entSize[2] = {32, 32};
    float entColor[4] = {255, 255, 255, 255};
    float entScale = 1.0f;
    bool entStatic = false;
    
    // Sprite properties
    int selectedMaterial = 0;
    float srcRegion[4] = {0, 0, 32, 32};
    bool flipX = false, flipY = false;
    
    // Placement mode
    bool mouseSpawnMode = true;
    bool showPreview = true;
    
    // Selected entity for editing
    Entity* selectedEntity = nullptr;
    
public:
    WorldEditor(World& world) : world(world) {
        InitializeDefaultTemplates();
    }
    
    void InitializeDefaultTemplates() {
        // Basic entity template
        EntityTemplate basic;
        basic.type = EntityType::ENTITY;
        basic.name = "Basic Entity";
        basic.color = {255, 0, 0, 255};
        templates.push_back(basic);
        
        // Platform template
        EntityTemplate platform;
        platform.type = EntityType::ENTITY;
        platform.name = "Platform";
        platform.size = {128, 16};
        platform.color = {128, 128, 128, 255};
        platform.isStatic = true;
        templates.push_back(platform);
        
        // Sprite template
        EntityTemplate sprite;
        sprite.type = EntityType::SPRITE_ENTITY;
        sprite.name = "Sprite";
        sprite.color = {255, 255, 255, 255};
        templates.push_back(sprite);
        
        // Tiled sprite template
        EntityTemplate tiled;
        tiled.type = EntityType::TILED_SPRITE_ENTITY;
        tiled.name = "Tiled Sprite";
        tiled.size = {128, 128};
        tiled.color = {255, 255, 255, 255};
        templates.push_back(tiled);
        
        // Player template
        EntityTemplate player;
        player.type = EntityType::PLAYER;
        player.name = "Player";
        player.hasAnimations = true;
        templates.push_back(player);
        
        currentTemplate = templates[0];
    }
    
    void LoadTemplateToUI(const EntityTemplate& tmpl) {
        entPos[0] = tmpl.position.x;
        entPos[1] = tmpl.position.y;
        entSize[0] = tmpl.size.x;
        entSize[1] = tmpl.size.y;
        entColor[0] = tmpl.color.r;
        entColor[1] = tmpl.color.g;
        entColor[2] = tmpl.color.b;
        entColor[3] = tmpl.color.a;
        entScale = tmpl.scale;
        entStatic = tmpl.isStatic;
        
        srcRegion[0] = tmpl.srcRect.x;
        srcRegion[1] = tmpl.srcRect.y;
        srcRegion[2] = tmpl.srcRect.w;
        srcRegion[3] = tmpl.srcRect.h;
        flipX = tmpl.flipX;
        flipY = tmpl.flipY;
    }
    
    EntityTemplate CreateTemplateFromUI() {
        EntityTemplate tmpl = currentTemplate;
        tmpl.position = {entPos[0], entPos[1]};
        tmpl.size = {entSize[0], entSize[1]};
        tmpl.color = {entColor[0], entColor[1], entColor[2], entColor[3]};
        tmpl.scale = entScale;
        tmpl.isStatic = entStatic;
        
        // Sprite properties
        auto materials = MaterialManager::GetInstance().GetMaterialNames();
        if (selectedMaterial >= 0 && selectedMaterial < materials.size()) {
            tmpl.materialName = materials[selectedMaterial];
        }
        tmpl.srcRect = {srcRegion[0], srcRegion[1], srcRegion[2], srcRegion[3]};
        tmpl.flipX = flipX;
        tmpl.flipY = flipY;
        
        return tmpl;
    }
    
    void SpawnEntity(const EntityTemplate& tmpl) {
        Entity* entity = tmpl.Spawn(world);
        if (entity) {
            world.AddEntity(entity);
            if (tmpl.type == EntityType::PLAYER) {
                SDL_Log("Player spawned successfully");
            }
        }
    }
    
    void Update(double dt, SDL_Renderer* renderer) {
        Camera& camera = Camera::GetInstance();
        
        // Mouse spawn mode
        if (mouseSpawnMode && ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse) {
            Vector2 mousePos = camera.ScreenToWorld(GetMousePosition());
            EntityTemplate tmpl = CreateTemplateFromUI();
            tmpl.position = mousePos;
            SpawnEntity(tmpl);
        }
        
        // Draw preview
        if (showPreview) {
            Vector2 mousePos = camera.ScreenToWorld(GetMousePosition());
            DrawPreview(renderer, mousePos);
        }
    }
    
    void DrawPreview(SDL_Renderer* renderer, Vector2 worldPos) {
        Camera& camera = Camera::GetInstance();
        Vector2 screenPos = camera.WorldToScreen(worldPos);
        
        SDL_FRect rect = {};
        rect.w = entSize[0] * camera.GetZoomOnScreen(entScale);
        rect.h = entSize[1] * camera.GetZoomOnScreen(entScale);
        rect.x = screenPos.x - rect.w * 0.5f;
        rect.y = screenPos.y - rect.h * 0.5f;
        
        SDL_SetRenderDrawColor(renderer, entColor[0], entColor[1], entColor[2], 128);
        SDL_RenderRect(renderer, &rect);
    }
    
    void DrawUI() {
        if (ImGui::Begin("Entity Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            
            // Template selector
            if (ImGui::BeginCombo("Template", templates[selectedTemplate].name.c_str())) {
                for (int i = 0; i < templates.size(); i++) {
                    bool isSelected = (selectedTemplate == i);
                    if (ImGui::Selectable(templates[i].name.c_str(), isSelected)) {
                        selectedTemplate = i;
                        currentTemplate = templates[i];
                        LoadTemplateToUI(templates[i]);
                    }
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            
            // Entity type selector
            const char* typeNames[] = {"Entity", "Sprite", "Tiled Sprite", "Animated", "Player"};
            int currentType = (int)currentTemplate.type;
            if (ImGui::Combo("Entity Type", &currentType, typeNames, IM_ARRAYSIZE(typeNames))) {
                currentTemplate.type = (EntityType)currentType;
            }
            
            ImGui::Separator();
            ImGui::Text("Properties");
            
            // Position & Size
            ImGui::SliderFloat2("Position", entPos, -1000, 1000, "%.1f");
            ImGui::SliderFloat2("Size", entSize, 1.0, 500.0, "%.1f");
            ImGui::SliderFloat("Scale", &entScale, 0.1f, 10.0f, "%.2f");
            
            // Color (only for non-sprite entities or tinting)
            float col[4] = {
                entColor[0]/255.0f, 
                entColor[1]/255.0f, 
                entColor[2]/255.0f, 
                entColor[3]/255.0f
            };
            if (ImGui::ColorEdit4("Color", col)) {
                entColor[0] = (int)(col[0] * 255);
                entColor[1] = (int)(col[1] * 255);
                entColor[2] = (int)(col[2] * 255);
                entColor[3] = (int)(col[3] * 255);
            }
            
            ImGui::Checkbox("Static", &entStatic);
            
            // Sprite-specific properties
            if (currentTemplate.type >= EntityType::SPRITE_ENTITY) {
                ImGui::Separator();
                ImGui::Text("Sprite Properties");
                
                auto materials = MaterialManager::GetInstance().GetMaterialNames();
                if (!materials.empty()) {
                    std::vector<const char*> materialCStrings;
                    for (const auto& name : materials) {
                        materialCStrings.push_back(name.c_str());
                    }
                    ImGui::Combo("Material", &selectedMaterial, materialCStrings.data(), materialCStrings.size());
                } else {
                    ImGui::Text("No materials loaded");
                }
                
                ImGui::SliderFloat4("Source Region", srcRegion, 0, 512, "%.0f");
                ImGui::Checkbox("Flip X", &flipX);
                ImGui::SameLine();
                ImGui::Checkbox("Flip Y", &flipY);
            }
            
            ImGui::Separator();
            
            // Spawn options
            ImGui::Checkbox("Mouse Spawn Mode", &mouseSpawnMode);
            ImGui::Checkbox("Show Preview", &showPreview);
            
            if (ImGui::Button("Spawn Here", ImVec2(200, 0))) {
                EntityTemplate tmpl = CreateTemplateFromUI();
                SpawnEntity(tmpl);
            }
            
            if (ImGui::Button("Save as Template", ImVec2(200, 0))) {
                EntityTemplate tmpl = CreateTemplateFromUI();
                static char nameBuf[64] = "Custom";
                ImGui::OpenPopup("Save Template");
            }
            
            // Save template popup
            if (ImGui::BeginPopupModal("Save Template", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                static char nameBuf[64] = "Custom Template";
                ImGui::InputText("Name", nameBuf, 64);
                
                if (ImGui::Button("Save")) {
                    EntityTemplate tmpl = CreateTemplateFromUI();
                    tmpl.name = nameBuf;
                    templates.push_back(tmpl);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            
            ImGui::Separator();
            
            // Material manager
            if (ImGui::CollapsingHeader("Material Manager")) {
                static char pathBuf[256] = "assets/textures/sprite.png";
                static char nameBuf[64] = "sprite";
                
                ImGui::InputText("Path", pathBuf, 256);
                ImGui::InputText("Name", nameBuf, 64);
                
                if (ImGui::Button("Load Material")) {
                    Material* mat = MaterialManager::GetInstance().LoadMaterial(nameBuf, pathBuf);
                    if (mat) {
                        SDL_Log("Material '%s' loaded successfully", nameBuf);
                    } else {
                        SDL_Log("Failed to load material '%s'", nameBuf);
                    }
                }
                
                ImGui::Text("Loaded Materials:");
                auto materials = MaterialManager::GetInstance().GetMaterialNames();
                for (const auto& name : materials) {
                    ImGui::BulletText("%s", name.c_str());
                }
            }
            
            // Level save/load
            if (ImGui::CollapsingHeader("Level Management")) {
                static char levelName[64] = "level1.txt";
                ImGui::InputText("Filename", levelName, 64);
                
                if (ImGui::Button("Save Level", ImVec2(150, 0))) {
                    SaveLevel(levelName);
                }
                ImGui::SameLine();
                if (ImGui::Button("Load Level", ImVec2(150, 0))) {
                    LoadLevel(levelName);
                }
            }
        }
        ImGui::End();
    }
    
    // TODO: Test if this works!
    void SaveLevel(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            SDL_Log("Failed to open file for writing: %s", filename.c_str());
            return;
        }
        
        // Write entity count
        const auto& entities = world.GetEntities();
        file << entities.size() << "\n";
        
        for (const Entity* entity : entities) {
            // Determine entity type
            EntityType type = EntityType::ENTITY;
            std::string materialName = "";
            SDL_FRect srcRect = {0, 0, 32, 32};
            
            if (const Player* player = dynamic_cast<const Player*>(entity)) {
                type = EntityType::PLAYER;
                if (player->GetMaterial()) {
                    materialName = player->GetMaterial()->GetPath();
                }
            } else if (const TiledSpriteEntity* tiled = dynamic_cast<const TiledSpriteEntity*>(entity)) {
                type = EntityType::TILED_SPRITE_ENTITY;
                if (tiled->GetMaterial()) {
                    materialName = tiled->GetMaterial()->GetPath();
                }
            } else if (const AnimatedEntity* animated = dynamic_cast<const AnimatedEntity*>(entity)) {
                type = EntityType::ANIMATED_ENTITY;
                if (animated->GetMaterial()) {
                    materialName = animated->GetMaterial()->GetPath();
                }
            } else if (const SpriteEntity* sprite = dynamic_cast<const SpriteEntity*>(entity)) {
                type = EntityType::SPRITE_ENTITY;
                if (sprite->GetMaterial()) {
                    materialName = sprite->GetMaterial()->GetPath();
                }
            }
            
            Vector2 pos = entity->GetPosition();
            Vector2 size = entity->GetSize();
            Color color = entity->GetColor();
            
            file << (int)type << " "
                 << pos.x << " " << pos.y << " "
                 << size.x << " " << size.y << " "
                 << color.r << " " << color.g << " " << color.b << " " << color.a << " "
                 << entity->GetScale() << " "
                 << entity->IsStatic() << " "
                 << materialName << "\n";
        }
        
        file.close();
        SDL_Log("Level saved: %s", filename.c_str());
    }
    
    // TODO: Test if this works!
    void LoadLevel(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            SDL_Log("Failed to open file for reading: %s", filename.c_str());
            return;
        }
        
        world.ClearEntities();
        
        int entityCount;
        file >> entityCount;
        
        for (int i = 0; i < entityCount; i++) {
            int typeInt;
            float px, py, sx, sy, scale;
            float r, g, b, a, isStatic;
            std::string materialName;
            
            file >> typeInt >> px >> py >> sx >> sy >> r >> g >> b >> a >> scale >> isStatic;
            std::getline(file >> std::ws, materialName);
            
            EntityTemplate tmpl;
            tmpl.type = (EntityType)typeInt;
            tmpl.position = {px, py};
            tmpl.size = {sx, sy};
            tmpl.color = {r, g, b, a};
            tmpl.scale = scale;
            tmpl.isStatic = (bool)isStatic;
            tmpl.materialName = materialName;
            
            SpawnEntity(tmpl);
        }
        
        file.close();
        SDL_Log("Level loaded: %s", filename.c_str());
    }
};