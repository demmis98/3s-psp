#ifndef PLAYER_H_   // include guard
#define PLAYER_H_

// libraries
#include "entity.h"

class Game;

// constants
#define PLAYER_Y_OFF 7

class Player: public Entity{
    public:
        int direction = 2;
        int fireTimer = 0;

        Player();
        Player(Game *game, Texture *texture, float x, float y);

        void logic() override;
        void draw() override;
};

#endif // PLAYER_H_