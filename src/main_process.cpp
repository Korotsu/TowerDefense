#include <main_process.h>
#include <Title_screen.h>
#include <stdio.h>
#include <Game.h>
#include <Death_screen.h>

screen::screen()
{
	sc_number = 1;
	score 		= 0;
	win 		= false;
	nb_enemy	= 0;
	wave_m_lvl 	= 1;
	wave 		= 1;
	music 		= NULL;

	// creation of the screen window
	sc = SDL_CreateWindow("Title screen", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, 1281, 833, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(sc, -1, SDL_RENDERER_PRESENTVSYNC);
	//Initialisation of SDL_mixor
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(2);
	}
}

screen::~screen()
{}

Time::Time()
{
	t 			= 0;
    prevTime 	= 0;
	deltaTime  	= 0.f;
	spawn_time  = 0; 
}

Time::~Time()
{}

screen* main_process()
{
	// creation de the event management system and the infinite loop
	screen* sc;
    sc = new screen;
	Time* t;
	t = new Time;
	//SDL_Event event;
	bool isGameRunning = true;
	while (isGameRunning)
	{
		//Exit
		if (sc->sc_number == 0)
		{
			return sc;
		}

		//Title screen
		else if (sc->sc_number == 1)
		{
			titleScreen(sc, t);
		}

		//Main game screen
		else if (sc->sc_number == 2)
		{
			game(sc, t);
		}

		//Death_screen
		else if (sc->sc_number == 3)
		{
			death_screen(sc,t);
		}
		
	}
	return sc;
}	