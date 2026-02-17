#ifndef BOMB_H_   // include guard
#define BOMB_H_

// libraries

// custom
#include "entity.h"

class Bomb: public Entity{
    public:
        Bomb(Game *game, Texture *texture, float x, float y);

};

#endif  // BOMB_H_