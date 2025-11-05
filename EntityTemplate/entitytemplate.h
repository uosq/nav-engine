#pragma once
#include <string>
#include <memory>

#include "../Entity/entity.h"
#include "../SpriteEntity/spriteentity.h"
#include "../AnimatedEntity/animatedentity.h"
#include "../TiledSpriteEntity/tiledspriteentity.h"
#include "../Player/player.h"

#include "../Material/materialmanager.h"
#include "../World/world.h"

enum class EntityType {
    ENTITY,
    SPRITE_ENTITY,
    TILED_SPRITE_ENTITY,
    ANIMATED_ENTITY,
    PLAYER
};

struct EntityTemplate {
    EntityType type;
    std::string name;
    
    // Common properties
    Vector2 position;
    Vector2 size;
    Color color;
    float scale;
    bool isStatic;
    
    // Sprite properties
    std::string materialName;
    SDL_FRect srcRect; // Sprite region
    bool flipX, flipY;
    
    // Tiled sprite properties
    bool isTiled;
    
    // Animation properties (for AnimatedEntity/Player)
    bool hasAnimations;
    
    EntityTemplate() 
        : type(EntityType::ENTITY)
        , name("Entity")
        , position{0, 0}
        , size{32, 32}
        , color{255, 255, 255, 255}
        , scale(1.0f)
        , isStatic(false)
        , materialName("")
        , srcRect{0, 0, 32, 32}
        , flipX(false)
        , flipY(false)
        , isTiled(false)
        , hasAnimations(false)
    {}
    
    Entity* Spawn(World& world) const {
        Entity* entity = nullptr;
        
        switch (type) {
            case EntityType::ENTITY:
                entity = new Entity();
                break;
                
            case EntityType::SPRITE_ENTITY: {
                SpriteEntity* sprite = new SpriteEntity();
                if (!materialName.empty()) {
                    Material* mat = MaterialManager::GetInstance().GetMaterial(materialName);
                    sprite->SetMaterial(mat);
                    sprite->SetRegion(srcRect.x, srcRect.y, srcRect.w, srcRect.h);
                }
                entity = sprite;
                break;
            }
            
            case EntityType::TILED_SPRITE_ENTITY: {
                TiledSpriteEntity* tiled = new TiledSpriteEntity();
                if (!materialName.empty()) {
                    Material* mat = MaterialManager::GetInstance().GetMaterial(materialName);
                    tiled->SetMaterial(mat);
                    tiled->SetRegion(srcRect.x, srcRect.y, srcRect.w, srcRect.h);
                }
                entity = tiled;
                break;
            }
            
            case EntityType::ANIMATED_ENTITY: {
                AnimatedEntity* animated = new AnimatedEntity();
                if (!materialName.empty()) {
                    Material* mat = MaterialManager::GetInstance().GetMaterial(materialName);
                    animated->SetMaterial(mat);
                }
                animated->InitializeAnimations();
                entity = animated;
                break;
            }
            
            case EntityType::PLAYER: {
                // Check if player already exists
                if (world.GetLocalPlayer() != nullptr) {
                    SDL_Log("Player already exists! Cannot spawn another.");
                    return nullptr;
                }
                Player* player = new Player();
                if (!materialName.empty()) {
                    Material* mat = MaterialManager::GetInstance().GetMaterial(materialName);
                    player->SetMaterial(mat);
                }
                player->InitializeAnimations();
                entity = player;
                break;
            }
        }
        
        if (entity) {
            entity->SetPosition(position);
            entity->SetSize(size);
            entity->SetColor(color);
            entity->SetScale(scale);
            entity->SetStatic(isStatic);
        }
        
        return entity;
    }
};