#include "bomb.h"
#include "entity.h"

Bomb :: Bomb(Game *game, Texture *texture, float x, float y) : Entity(game, texture, x, y){
    ySpeed = 3;
    hitbox.off_x = hitbox.w / 4;
    hitbox.off_y = hitbox.h / 4;
    hitbox.w /= 2;
    hitbox.h /= 2;

    mortal = true;

    dieIfOffScreen = true;

    team = TEAM_ENEMY;

    hurts = true;
    damage = 1.1f;
    
    useHitbox = true;
}
