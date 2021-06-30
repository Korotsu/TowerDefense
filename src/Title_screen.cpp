#include <Title_screen.h>
#include <my_put_string.h>
#include <iostream>

void titleScreen(screen* sc, Time* t)
{
	SDL_SetRenderDrawColor(sc->renderer, 255, 255, 255, 255);
    pic picture(sc);
	title_print_text(sc, picture);
	// creation de the event management system and the infinite loop
	if( Mix_PlayingMusic() == 1 )
    {
		Mix_HaltMusic();
    }
	sc->music = Mix_LoadMUS( "./media/Sound/Music/welcome to the jungle.mp3" );
	Mix_PlayMusic( sc->music, -1 );
	Mix_VolumeMusic(100);
	int x = 0;
	int y = 0;
	SDL_Event event;
	bool isGameRunning = true;
	while (isGameRunning)
	{
		while (SDL_PollEvent(&event))
		{
			t->t = SDL_GetTicks(); // current time in milliseconds
			if (t->t > t->prevTime)
			{
				t->deltaTime = (t->t - t->prevTime) / 1000.f; // convert deltatime in seconds
				t->prevTime = t->t;
			}

			//record states of mouse
		    	Uint32 mouseState = SDL_GetMouseState(&x, &y);
		   	 bool leftClicked = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

			// allow the player to close the game with the escape keyboard touch
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
			{
				isGameRunning = false;
				SDL_DestroyTexture(picture.texture);
				sc->sc_number = 0;
			}
			
			else if ( x >= 440 && x <= 840 && y >= 350 && y <= 410 && leftClicked)
			{
				isGameRunning = false;
				SDL_DestroyTexture(picture.texture);
				sc->sc_number = 2;
			}
		}
	}
}

//function to create the text on the screen
pic::pic(screen* sc)
{
	//load the picture
	surface = IMG_Load("./media/Picture/fond_du_td.png");
	rect = {0,0,1281,833};
	src = {0,0,1281,833};
	texture = SDL_CreateTextureFromSurface(sc->renderer, surface);
	SDL_FreeSurface(surface);
}

pic::~pic()
{}

void title_print_text(screen* sc, pic picture )
{		
	SDL_SetRenderDrawColor(sc->renderer, 0, 0, 0, 255);
	SDL_RenderClear(sc->renderer);
	SDL_RenderCopy(sc->renderer, picture.texture, &(picture.src),&(picture.rect));
	if(TTF_Init() == -1)
	{
		my_put_string("Erreur d'initialisation de TTF_Init : %s\n");
		my_put_string(TTF_GetError());
	}
	sc->font  = TTF_OpenFont("./media/Fonts/font.ttf", 300);
	SDL_Color color = { 255, 255, 255, 255 };

    SDL_Rect rect1 = { 440, 350, 400, 60 };
    SDL_RenderDrawRect(sc->renderer,&rect1);
	SDL_SetRenderDrawColor(sc->renderer,100,1,1,1);
	SDL_RenderFillRect(sc->renderer,&rect1);
    color = {230,230,230,255};
	const char* word = " START ROCKING !";
	SDL_Surface* surface2 = TTF_RenderText_Blended(sc->font, word, color);
	SDL_Texture* texture2 = SDL_CreateTextureFromSurface(sc->renderer, surface2);
	SDL_FreeSurface(surface2);

    SDL_Rect rect2 = { 440, 500, 400, 60 };
    SDL_RenderDrawRect(sc->renderer,&rect2);
	//SDL_SetRenderDrawColor(screen->renderer,1,1,200,1);
	SDL_RenderFillRect(sc->renderer,&rect2);
    SDL_SetRenderDrawColor(sc->renderer,255,255,255,1);
	word = " Level editor (WIP) !";
	SDL_Surface* surface3 = TTF_RenderText_Blended(sc->font, word, color);
	SDL_Texture* texture3 = SDL_CreateTextureFromSurface(sc->renderer, surface3);
	SDL_FreeSurface(surface3);

    SDL_SetRenderDrawColor(sc->renderer,130,90,50,1);
	SDL_RenderCopy(sc->renderer, texture2, NULL, &(rect1));
	SDL_RenderCopy(sc->renderer, texture3, NULL, &(rect2));
	SDL_RenderPresent(sc->renderer);
}