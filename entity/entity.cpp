#include "entity.h"

#include "../game/game.h"
#include "explode.h"

Entity :: Entity(){
	
}

Entity :: Entity(Game *game, Texture *texture) : Entity(game, texture, 0.0f, 0.0f) {
}

Entity :: Entity(Game *game, Texture *texture, float x, float y){
	this->game = game;
	this->texture = texture;
	this->x = x;
	this->y = y;

	hitbox.w = texture->width;
	hitbox.h = texture->height;
}

void Entity :: move(){
	x += xSpeed;
	y += ySpeed;
	updateHitbox();
}

void Entity :: moveAndCollide(){
	bool moveX = true;
	bool moveY = true;
	xTemp = x;
	yTemp = y;

	float xD = xSpeed;
	float yD = ySpeed;
	if(xD < 0)
		xD = -xD;
	if(yD < 0)
		yD = -yD;
	int n = xD + yD + 2;
	xD = xSpeed / n;
	yD = ySpeed / n;
	
	for(int i = 0; i < n && (moveX || moveY); i++){
		if(moveX)
			x += xD;
		if(collition()){
			x -= xD;
			moveX = false;
		}
		if(moveY)
			y += yD;
		if(collition()){
			y -= yD;
			moveY = false;
		}
	}

	updateHitbox();
}

void Entity :: updateHitbox(){
	hitbox.x = x + hitbox.off_x;
	hitbox.y = y + hitbox.off_y;
}

void Entity :: updateRenderXY(){
	xRender = x;
	yRender = y;
}

bool Entity :: collides(Box b1, Box b2){
	return (abs((b1.x + b1.w/2) - (b2.x + b2.w/2)) * 2 < (b1.w + b2.w)) &&
         (abs((b1.y + b1.h/2) - (b2.y + b2.h/2)) * 2 < (b1.h + b2.h));
}

bool Entity :: collition(){
	bool r = false;
	auto end = game->level.entities.end();
	if(solid){
		updateHitbox();
		for(auto i = game->level.entities.begin(); i != end && *i != this; ++i){
			if((*i)->useHitbox)
			if(team != (*i)->team)
			if((*i)->solid)
			if(collides(hitbox, (*i)->hitbox)){
				r = true;
			}
		}
	}

	updateRenderXY();
	if(alwaysOnScreen){
		if(!isFullyOnScreen())
			r = true;
	}

	return r;
}

bool Entity :: hitDetect(){
	bool r = false;
	auto end = game->level.entities.end();
	if(useHitbox){
		for(auto i = game->level.entities.begin(); i != end && *i != this; ++i){
			if((*i)->useHitbox)
			if(team != (*i)->team)
			if(collides(hitbox, (*i)->hitbox)){
				//drawTextureF(game->t_bullet, 50, 50);
				(*i)->hit(this);
				this->hit(*i);
				r = true;
				//r = true;
			}
		}
	}

	return r;
}

void Entity :: hit(Entity *e){
	if(e->hurts && mortal){
		life -= e->damage;
		if(life <= 0.0f && life + e->damage > 0)
			game->level.entities.push_back(new Explode(game, x, y, texture->width, texture->height));
	}
}

bool Entity :: isOnScreen(){
	return xRender + texture->width >= -16 && xRender <= SCREEN_WIDTH + 16 && yRender + texture->height >= -16 && yRender <= SCREEN_HEIGHT + 16;
}

bool Entity :: isFullyOnScreen(){
	return xRender > 0 && xRender + texture->width < SCREEN_WIDTH && yRender > 0 && yRender + texture->height < SCREEN_HEIGHT;
}

void Entity :: logic(){	
	if(useHitbox){
		moveAndCollide();
		hitDetect();
	}
	else{
		move();
	}
	updateRenderXY();

	if(dieIfOffScreen)
		if(!isOnScreen())
			life = 0.0f;

}

void Entity :: draw(){
	drawTextureF(texture, xRender, yRender);
}
