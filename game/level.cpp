#include "level.h"

#include "../inc/graphics.h"

#include "../entity/player.h"
#include "../entity/plane.h"

#include "game.h"
#include <list>
#include <memory>

Level :: Level(){}

Level :: Level(Game *game){
    this->game = game;

    initRandom();

    setBgColor(0xFFffd84d);
    grassY = LEVEL_GROUND_Y - game->t_grass->height;
    state = LEVEL_STATE_DIED;
}

void Level :: restart(){
    entities.push_back(new Player(this->game, game->t_tank, SCREEN_WIDTH / 2, LEVEL_GROUND_Y - game->t_tank->height));

    maxPlaneTimer = 230;
    minPlaneTimer = 120;
    
    state = LEVEL_STATE_PLAY;
}


void Level :: logic(){
    if(state == LEVEL_STATE_PLAY){
        if(entities.front()->life <= 0.0f){            
            entities.remove_if([](Entity *e){
                delete e;
                return true;
            });
            state = LEVEL_STATE_DIED;
        }

        entities.remove_if([](Entity *e){
            if(e->life <= 0.0f && e->mortal){
                delete e;
                return true;
            }
            return false;
        });

        planesTimer--;
        if(planesTimer <= 0){
            int d = randomD(0, 1);

            if(d)
                d = PLANE_FROM_LEFT;
            else
                d = PLANE_FROM_RIGHT;

            if(randomD(0, 2))
                entities.push_back(new Plane(game, game->t_plane_2, d, randomD(2,4) - 0.5, 1.0f));
            else
                entities.push_back(new Plane(game, game->t_plane, d, randomD(2,4) - 0.5, 10.0f));

            entities.back()->y = randomD(50, 100);

            planesTimer = randomD(minPlaneTimer, maxPlaneTimer);

            if(minPlaneTimer > 30)
                minPlaneTimer -= 5;

            if(maxPlaneTimer > 120)
                maxPlaneTimer -= 5;
        }

        for(auto &i : entities){
            i->logic();
        }

        grassX -= 0.7f;

        if(grassX < -game->t_grass->width){
            grassX = 0.0f;
        }

        mountainX -= 0.15f;
        if(mountainX < -game->t_mountain->width){
            mountainX = 0.0f;
        }
    }
    else if(state == LEVEL_STATE_DIED){
        if(game->buttons.a && !game->buttonsTemp.a){
            restart();
            state = LEVEL_STATE_PLAY;
        }
    }
}

void Level :: drawBg(Texture *texture, float bgX, float bgY){
    float w = texture->width;
    setTexture(texture, GU_TFX_REPLACE);
    for(float i = bgX; i < SCREEN_WIDTH; i += w){
        drawTextureSet(i, bgY, 0, 0, i + texture->width, bgY + texture->height, texture->width, texture->height, 0xFFFFFFFF);
    }
}

void Level :: draw(){
    // draw scrolling bg;
    //drawBg(game->t_mountain, mountainX, grassY - game->t_mountain->height + 10);
    drawTextureH(game->t_mountain, mountainX, grassY - game->t_mountain->height + 20, 0xFFFFFFFF);
    //drawBg(game->t_grass, grassX, grassY);
    drawTextureH(game->t_grass, grassX, grassY, 0xFFFFFFFF);
    //drawBg(game->t_floor, 0, LEVEL_GROUND_Y);
    drawTextureH(game->t_floor, 0, LEVEL_GROUND_Y, 0xFFFFFFFF);
    if(state == LEVEL_STATE_DIED)
        drawTextureF(game->t_press_x, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    for(auto &i : entities)
        i->draw();
}