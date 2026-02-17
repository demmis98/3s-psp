#ifndef GAME_H_	// incluge guard
#define GAME_H_ 1

// libraries
#include <pspctrl.h>

// custom
#include "../inc/graphics.h"
#include "../inc/audio.h"
#include "level.h"

//class Level;

typedef struct{
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	bool a = false;
	bool b = false;

	bool start = false;
} gameButtons;

class Game{
	public:
		gameButtons buttons, buttonsTemp;
		bool pause = false;

		Texture *t_tank;
		Texture *t_gun[5];

		Texture *t_bullet;
		Texture *t_plane;
		Texture *t_plane_2;
		Texture *t_bomb;

		Texture *t_explode[4];
		
		Texture *t_grass;
		Texture *t_mountain;
		Texture *t_floor;

		Wav *w_bullet;
		Wav *w_explode;
		Wav *w_hit;

		Level level;

		Game();

		void readController();
		void init();
		void logic();
		void draw();

	private:	
		SceCtrlData pad;
};

#endif	// GAME_H_
