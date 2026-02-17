#include "bullet.h"
#include "entity.h"

Bullet :: Bullet(Game *game, Texture *texture, float x, float y, float xSpeed, float ySpeed) : Entity(game, texture, x, y){
    this->xSpeed = xSpeed;
    this->ySpeed = ySpeed;

    mortal = true;

    dieIfOffScreen = true;
    hurts = true;
    damage = 2.0f;
    
    useHitbox = true;
}

void Bullet :: hit(Entity *e){
    life = 0.0f;
}

