#pragma once

#include <vector>
#include "../Entity/entity.h"
#include "../Player/player.h"

class World {
    private:
    std::vector<Entity*> entitylist;
    int max_entities = 64;
    Player *localPlayer;

    World(const World&) = delete;
    World& operator=(const World&) = delete;

    World() {
        entitylist.reserve(max_entities);
    };

    public:

    static World& GetInstance() {
        static World world;
        return world;
    }

    std::vector<Entity*> GetEntities() {
        return entitylist;
    }

    bool AddEntity(Entity *entity) {
        if (entitylist.size() == max_entities)
            return false;
        entitylist.push_back(entity);
        return true;
    }

    bool RemoveEntity(Entity *entity) {
        if (entitylist.size() == 0)
            return false;

        for (int i = 0; i < entitylist.size(); i++) {
            Entity *e = entitylist.at(i);
            if (e == entity && e != nullptr) {
                entitylist.erase(entitylist.begin() + i);
                delete e;
                e = nullptr;
                break;
            }
        }

        return true;
    }

    void ProcessEntities(double dt) {
        if (entitylist.size() == 0)
            return;

        for (auto *e : entitylist) {
            if (!e || e == nullptr)
                continue;

            e->Process(dt);
        }
    }

    void DrawEntities() {
        if (entitylist.size() == 0)
            return;

        for (auto *e : entitylist) {
            if (!e || e == nullptr)
                continue;

            e->Draw();
        }
    }

    void SetMaxEntities(int max_entities) {
        this->max_entities = max_entities;
    }

    Player* GetLocalPlayer() {
        return localPlayer;
    }

    void SetLocalPlayer(Player *player) {
        localPlayer = player;
    }

    void ClearEntities() {
        for (Entity *e : entitylist)
            delete e;
        entitylist.clear();
    }
};