#ifndef TITLE_SCREEN_H
#define TITLE_SCREEN_H

// include all the needed librairies
#include <main_process.h>

//create struct needed for the title sc
class pic
{
public:
	SDL_Surface* surface;
	SDL_Texture* texture;
	SDL_Rect rect;
	SDL_Rect src;
	pic(screen* sc);
	~pic();

};

// create the definition of all the title sc function;

void title_print_text(screen* sc, pic pic);

void 		titleScreen(screen* sc, Time* time);

#endif
