#include "player.h"

#include "bullet.h"
#include "entity.h"
#include "../game/game.h"

Player :: Player() : Entity(){

}

Player :: Player(Game *game, Texture *texture, float x, float y) : Entity(game, texture, x, y){
	hitbox.off_y = PLAYER_Y_OFF;
	hitbox.h -= PLAYER_Y_OFF;

	mortal = true;
	life = 4.0f;
	alwaysOnScreen = true;
    useHitbox = true;
}

void Player :: logic(){
	if(game->buttons.right){
		xSpeed = 3.5;

		if(game->buttons.up)
			direction = 3;
		else
			direction = 4;
	}
	else if(game->buttons.left){
		xSpeed = -3.5;

		if(game->buttons.up)
			direction = 1;
		else
		 	direction = 0;
	}
	else {
		xSpeed = 0;

		if(game->buttons.up)
			direction = 2;
	}

	if(game->buttons.a){
		fireTimer--;
		if(fireTimer < 0){
			// level add bullet
			float bullX = x + texture->width/2 - 4;
			float bullY = y + 10;
			float bullXSpeed = 0.0f;
			float bullYSpeed = 0.0f;
			float bullSpeed = 5.0f;
			if(direction == 0){
				bullXSpeed = -bullSpeed;
			}
			else if(direction == 1){
				bullXSpeed = -bullSpeed * INV_ROOT_2;
				bullYSpeed = -bullSpeed * INV_ROOT_2;
			}
			else if(direction == 2){
				bullYSpeed = -bullSpeed;
			}
			else if(direction == 3){
				bullXSpeed = bullSpeed * INV_ROOT_2;
				bullYSpeed = -bullSpeed * INV_ROOT_2;
			}
			else if(direction == 4){
				bullXSpeed = bullSpeed;
			}
			bullX += bullXSpeed * 3;
			bullY += bullYSpeed * 2;
			bullXSpeed += xSpeed;
			game->level.entities.push_back(new Bullet(game, game->t_bullet, bullX, bullY, bullXSpeed, bullYSpeed));
			playWav(game->w_bullet, 1);
			fireTimer = 8;
		}
	}
	else{
		if(fireTimer)
			fireTimer--;
	}

    Entity::logic();
}

void Player :: draw(){
	drawTextureF(game->t_gun[direction], xRender, yRender);
	Entity::draw();
}