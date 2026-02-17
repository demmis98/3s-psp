#include "explode.h"

#include "../game/game.h"
#include "entity.h"

Explode :: Explode(Game *game, float x, float y, float xScale, float yScale)
    : Entity(game, game->t_explode[0], x, y){
    this->xScale = xScale;
    this->yScale = yScale;

    mortal = true;
    playWav(game->w_explode, 2);
}

void Explode :: logic(){
    Entity::logic();
    timer++;
    if(timer == 10){
        index++;
        if(index >= 4)
            life = 0.0f;
        else
            texture = game->t_explode[index];
        timer = 0;
    }
}

void Explode :: draw(){
    drawTexture(texture, xRender, yRender, 0.0f, 0.0f, xRender + xScale, yRender + yScale, texture->width, texture->height, 0xFFFFFFFF);
}