#ifndef BULLET_H_   // include guard
#define BULLET_H_

// libraries

// custom
#include "entity.h"

class Bullet: public Entity{

    public:
        Bullet(Game *game, Texture *texture, float x, float y, float xSpeed, float ySpeed);

        void hit(Entity *e) override;
};

#endif  // BULLET_H_