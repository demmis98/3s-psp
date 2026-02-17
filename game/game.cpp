#include "../inc/graphics.h"

#include "game.h"
//#include "level.h"

Game :: Game(){
	// set sampling for reading inputs
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    
    // start rendering
    //initGu();
}

void Game :: init(){
	t_tank = loadTexture("assets/img/tank.png");

	t_gun[0] = loadTexture("assets/img/gun_0.png");
	t_gun[1] = loadTexture("assets/img/gun_1.png");
	t_gun[2] = loadTexture("assets/img/gun_2.png");
	t_gun[3] = loadTexture("assets/img/gun_3.png");
	t_gun[4] = loadTexture("assets/img/gun_4.png");
	t_bullet = loadTexture("assets/img/bullet.png");

	t_plane = loadTexture("assets/img/plane.png");
	t_plane_2 = loadTexture("assets/img/plane_2.png");
	t_bomb = loadTexture("assets/img/bomb.png");

	t_explode[0] = loadTexture("assets/img/explode_0.png");
	t_explode[1] = loadTexture("assets/img/explode_1.png");
	t_explode[2] = loadTexture("assets/img/explode_2.png");
	t_explode[3] = loadTexture("assets/img/explode_3.png");

	
	t_grass = loadTexture("assets/img/grass.png");
	t_mountain = loadTexture("assets/img/mountain.png");
	t_floor = loadTexture("assets/img/floor.png");

	t_press_x = loadTexture("assets/img/press x.png");

	w_bullet = loadWav("assets/sound/tankfire.wav");
	w_explode = loadWav("assets/sound/explode.wav");
	w_hit = loadWav("assets/sound/bullethit.wav");

	level = Level(this);
}

void Game :: readController(){
	// read controler state
	sceCtrlReadBufferPositive(&pad, 1);

	buttons.up = pad.Buttons & PSP_CTRL_UP;
	buttons.down = pad.Buttons & PSP_CTRL_DOWN;
	buttons.left = pad.Buttons & PSP_CTRL_LEFT;
	buttons.right = pad.Buttons & PSP_CTRL_RIGHT;

	buttons.a = pad.Buttons & PSP_CTRL_CROSS;
	buttons.b = pad.Buttons & PSP_CTRL_SQUARE;

	buttons.start = pad.Buttons & PSP_CTRL_START;
}

void Game :: logic(){
	readController();

	if(!pause)
		level.logic();

	if(buttons.start && !buttonsTemp.start)
		pause = !pause;

	buttonsTemp = buttons;
}

void Game :: draw(){
	//drawTexture(t_tank, 100, 200, 32, 32);
	level.draw();
}