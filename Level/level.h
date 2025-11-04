#pragma once

#include "../World/world.h"

class Level {
    Level() {}

    public:
    /* Singleton */
    static Level& GetInstance() {
        static Level level;
        return level;
    }

    void Load();
    void Unload();
};