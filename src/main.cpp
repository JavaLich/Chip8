#include <SDL2/SDL.h>
#include <iostream>

#include "Chip8.h"

int main() {
  Chip8 chip;
  chip.load_rom("file");

  using std::cerr;
  using std::endl;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    cerr << "SDL_Init Error: " << SDL_GetError() << endl;
    return EXIT_FAILURE;
  }

  SDL_Window *win =
      SDL_CreateWindow("Chip8 Emulator", 0, 0, 640, 320, SDL_WINDOW_SHOWN);
  if (win == nullptr) {
    cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
    return EXIT_FAILURE;
  }

  SDL_Renderer *ren = SDL_CreateRenderer(
      win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ren == nullptr) {
    cerr << "SDL_CreateRenderer Error" << SDL_GetError() << endl;
    if (win != nullptr) {
      SDL_DestroyWindow(win);
    }
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_Texture *buffer = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888,
                                          SDL_TEXTUREACCESS_STREAMING, 64, 32);

  while (true) {
    SDL_UpdateTexture(buffer, NULL, &chip.video, 64 * sizeof(uint32_t));
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, buffer, NULL, NULL);
    SDL_RenderPresent(ren);
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        break;
      }
    }
  }

  SDL_DestroyTexture(buffer);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return EXIT_SUCCESS;
}
