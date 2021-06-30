#include <Death_screen.h>
#include <my_put_string.h>
#include <string_length.h>
#include <int_to_alpha.h>

void death_screen(screen* screen, Time* t)
{
	SDL_Event event;
	text_init_d(screen);
	if( Mix_PlayingMusic() == 1 )
    {
		Mix_HaltMusic();
    }
	screen->music = Mix_LoadMUS( "./media/Sound/Music/(Lost) Aion OST - Redwood.mp3" );
	Mix_PlayMusic( screen->music, -1 );
	Mix_VolumeMusic(100);
	bool isGameRunning = true;
    int x = 0;
    int y = 0;
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
				screen->sc_number = 0;
			}

			else if ( x >= 440 && x <= 840 && y >= 300 && y <= 360 && leftClicked) 
			{
				isGameRunning = false;
				screen->sc_number = 2;
				screen->wave = 1;
			}

			else if ( x >= 440 && x <= 840 && y >= 500 && y <= 560 && leftClicked)
			{
				isGameRunning = false;
				screen->sc_number = 1;
				screen->wave = 1;
			}
		}
	}
}

void text_init_d(screen* screen)
{
    SDL_SetRenderDrawColor(screen->renderer, 0, 0, 0, 255);
	SDL_RenderClear(screen->renderer);
	SDL_SetWindowTitle(screen->sc, "(-_-)GAME OVER !(-_-)");
	if(TTF_Init() == -1)
	{
		my_put_string("Erreur d'initialisation de TTF_Init : %s\n");
		my_put_string(TTF_GetError());
	}
	screen->font  = TTF_OpenFont("./media/Fonts/font.ttf", 300);
	SDL_Color color = { 255, 255, 255, 255 };

    const char* word = "GAME OVER !";
	SDL_Surface* surface = TTF_RenderText_Blended(screen->font, word, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(screen->renderer, surface);
	SDL_FreeSurface(surface);
	SDL_Rect rect = { 440, 70, 400, 60 };

    print_wawe_number_reach(screen);

    SDL_Rect rect2 = { 440, 300, 400, 60 };
    SDL_RenderDrawRect(screen->renderer,&rect2);
	SDL_SetRenderDrawColor(screen->renderer,100,1,1,1);
	SDL_RenderFillRect(screen->renderer,&rect2);
    color = {230,230,230,1};
	word = " PLAY AGAIN !";
	SDL_Surface* surface2 = TTF_RenderText_Blended(screen->font, word, color);
	SDL_Texture* texture2 = SDL_CreateTextureFromSurface(screen->renderer, surface2);
	SDL_FreeSurface(surface2);

    SDL_Rect rect3 = { 440, 500, 400, 60 };
    SDL_RenderDrawRect(screen->renderer,&rect3);
	//SDL_SetRenderDrawColor(screen->renderer,1,1,200,1);
	SDL_RenderFillRect(screen->renderer,&rect3);
    SDL_SetRenderDrawColor(screen->renderer,255,255,255,1);
	word = " MAIN MENU !";
	SDL_Surface* surface3 = TTF_RenderText_Blended(screen->font, word, color);
	SDL_Texture* texture3 = SDL_CreateTextureFromSurface(screen->renderer, surface3);
	SDL_FreeSurface(surface3);

    SDL_SetRenderDrawColor(screen->renderer,130,90,50,1);
	SDL_RenderCopy(screen->renderer, texture, NULL, &(rect));
	SDL_RenderCopy(screen->renderer, texture2, NULL, &(rect2));
	SDL_RenderCopy(screen->renderer, texture3, NULL, &(rect3));
	SDL_RenderPresent(screen->renderer);
}

void print_wawe_number_reach(screen* screen)
	{
        SDL_Color color1 = { 255, 255, 255, 1};
        const char* word = "Congratulations, you reach the wave :";
		SDL_Surface* text_surface = TTF_RenderText_Blended(screen->font, word, color1);
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(screen->renderer, text_surface);
		SDL_FreeSurface(text_surface);
		SDL_Rect rect = { 50, 200, 20 * static_cast<int>(string_length(word)), 40 };
		const char* wave_text = int_to_alpha(screen->wave);
		SDL_Surface* surface = TTF_RenderText_Blended(screen->font, wave_text, color1);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(screen->renderer, surface);
		SDL_FreeSurface(surface);
		SDL_Rect rect_s = {20 * static_cast<int>(string_length(word)) + 60 , 200, 50 * static_cast<int>(string_length(wave_text)), 40};
		SDL_RenderCopy(screen->renderer,text_texture,NULL,&rect);
		SDL_RenderCopy(screen->renderer,texture, NULL, &rect_s);
		SDL_DestroyTexture(text_texture);
		SDL_DestroyTexture(texture);
	}