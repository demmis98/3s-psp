#ifndef PLANE_H_    // include guard
#define PLANE_H_

// libraries
#include "entity.h"

class Game;

// constants
#define PLANE_FROM_LEFT 0
#define PLANE_FROM_RIGHT 1

#define PLANE_HAS_NOT_BOMBED -42

class Plane: public Entity{
    public:
        int direction;
        int bombTimer = PLANE_HAS_NOT_BOMBED + 1;

        u32 colour = 0xFFFFFFFF;
        int colourTimer = 0;

        Plane();
        Plane(Game *game, Texture *texture, int direction, int speed, float life);

        void hit(Entity *e) override;
        void logic() override;
        void draw() override;

};

#endif