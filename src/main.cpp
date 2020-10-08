#include <SDL2/SDL.h>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>

#include "Chip8.h"

int main()
{
	Chip8 chip;
	chip.load_rom("file");

    using std::cerr;
    using std::endl;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cerr << "SDL_Init Error: " << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    SDL_Window* win = SDL_CreateWindow("Hello World!", 100, 100, 620, 387, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    SDL_Renderer* ren
        = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr) {
        cerr << "SDL_CreateRenderer Error" << SDL_GetError() << endl;
		if (win != nullptr) {
			SDL_DestroyWindow(win);
		}
		SDL_Quit();
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 20; i++) {
        SDL_RenderClear(ren);
        SDL_RenderPresent(ren);
        SDL_Delay(100);
    }
	
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return EXIT_SUCCESS;
}

