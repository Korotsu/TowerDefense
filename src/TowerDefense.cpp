#include <main_process.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	// initialization of SDL graphic library
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		printf("SDL_Init error: %s", SDL_GetError());
		return 0;
	}

	atexit(SDL_Quit);
	//Launch the main process
	screen* sc = main_process();
	//then destroy the screen
	SDL_DestroyWindow(sc->sc);
	SDL_DestroyRenderer(sc->renderer);
	Mix_FreeMusic(sc->music);
	free(sc);
	Mix_CloseAudio();

	return 0;
}