#ifndef EXPLODE_H_   // include guard
#define EXPLODE_H_

// libraries

// custom
#include "entity.h"

class Explode : public Entity{
    public:
        float xScale, yScale;
        int timer = 0;
        int index = 0;

        Explode(Game *game, float x, float y, float xScale, float yScale);
        void logic() override;
        void draw() override;
};

#endif