#ifndef MAIN_PROCESS_H
#define MAIN_PROCESS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

class screen
{
public:
	unsigned int 	sc_number;
	SDL_Window* 	sc;
	SDL_Renderer* 	renderer;
	bool 			win;
	TTF_Font*		font;
	int 			score;
	unsigned int 	wave;
	unsigned int 	wave_m_lvl;
	unsigned int 	nb_enemy;
	Mix_Music*		music;
	screen();
	~screen();
};

class Time
{
public:
	int 	t;
	int 	prevTime;
	float 	deltaTime;
	float 	time_paused;
	float 	spawn_time;
	Time();
	~Time();
};

screen* 	main_process();

#endif
