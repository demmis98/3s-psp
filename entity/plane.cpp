#include "plane.h"

#include "../game/game.h"
#include "entity.h"
#include "bomb.h"

Plane :: Plane() : Entity(){}

Plane :: Plane(Game *game, Texture *texture, int direction, int speed, float life) : Entity (game, texture){
    this->direction = direction;
    this->life = life;

    mortal = true;
    
    team = TEAM_ENEMY;
    hurts = true;

    useHitbox = true;
    dieIfOffScreen = true;

    
    if(direction == PLANE_FROM_LEFT){
        xSpeed = speed;
        x = -texture->width;
    }
    else if(direction == PLANE_FROM_RIGHT){
        xSpeed = -speed;
        x = SCREEN_WIDTH;
    }
}

void Plane :: hit(Entity *e){
    Entity::hit(e);
    colourTimer = 8;
    if(life > 0.0f)
        playWav(game->w_hit, 3);
}

void Plane :: logic(){
    Entity::logic();

    bombTimer--;
    if(bombTimer <= 0){
        if(bombTimer != PLANE_HAS_NOT_BOMBED)
            game->level.entities.push_back(new Bomb(game, game->t_bomb, x + texture->width/2 + xSpeed + xSpeed, y + 10));
        bombTimer = randomD(60, 120);
    }

    if(colourTimer > 0){
        colour = 0xFFFFFFFF - (0x00111100 * colourTimer);
        colourTimer--;
    }
    else {
        colour = 0xFFFFFFFF;
    }
}

void Plane :: draw(){
    if(colourTimer)
        setTexture(texture, GU_TFX_MODULATE);
    else
        setTexture(texture, GU_TFX_REPLACE);
    if(direction == PLANE_FROM_RIGHT)
        drawTextureSet(xRender, yRender,  0.0f, 0.0f
            , xRender + texture->width, yRender + texture->height, texture->width, texture->height, colour);
    else
        // draw plane fliped horizontaly
        drawTextureSet(xRender, yRender,  texture->width, 0.0f
            , xRender + texture->width, yRender + texture->height, 0.0f, texture->height, colour);
}