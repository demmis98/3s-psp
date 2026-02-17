#ifndef LEVEL_H_    // include guard
#define LEVEL_H_

// libraries
#include <list>

// custom
#include "../inc/math.h"

#include "../entity/entity.h"

// constants
#define LEVEL_GROUND_Y 232

#define LEVEL_STATE_PLAY 0
#define LEVEL_STATE_DIED 1

class Level{
    public:
        Game *game;

        int state;
        
        std::list<Entity*> entities;

        int planesTimer = 0;
        int maxPlaneTimer;
        int minPlaneTimer;
        float grassX;
        float grassY;
        float mountainX;

        Level();
        Level(Game *game);

        void restart();
        void logic();
        void drawBg(Texture *texture, float bgX, float bgY);
        void draw();
};

#endif  // LEVEL_H_