#ifndef ENTITY_H_	// include guard
#define ENTITY_H_ 1

// constants
#define TEAM_PLAYER 0
#define TEAM_ENEMY 1

// libraries

// custom
#include "../inc/math.h"
#include "../inc/graphics.h"

class Game;

typedef struct{
	int x = 0;
	int y = 0;
	int off_x = 0;
	int off_y = 0;
	int w = 16;
	int h = 16;
} Box;

class Entity{
	public:
	
		Game *game;
		
		float x;
		float y;
		float xSpeed = 0;
		float ySpeed = 0;
		float xRender = 0;
		float yRender = 0;

		bool mortal = false;
		float life = 1.0f;
		
		bool alwaysOnScreen = false;
		bool dieIfOffScreen = false;

		int team = TEAM_PLAYER;
		bool hurts = false;
		float damage = 0.0f;

		bool solid = false;
		bool useHitbox = false;

		Box hitbox;
		Texture *texture;

		Entity ();
		Entity (Game *game, Texture *texture);
		Entity (Game *game, Texture *texture, float x, float y);
		
		virtual void move();
		virtual void moveAndCollide();
		virtual void updateHitbox();
		virtual void updateRenderXY();
		virtual bool collides(Box b1, Box b2);
		virtual bool collition();
		virtual void hit(Entity *e);
		virtual bool hitDetect();
		virtual bool isOnScreen();
		virtual bool isFullyOnScreen();
		virtual void logic();
		virtual void draw();

	private:
		float xTemp;
		float yTemp;
};

#endif	// ENTITY_H_
