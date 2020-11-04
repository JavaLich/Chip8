#include <SDL2/SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_render.h>
#include <cstdlib>
#include <iostream>
#include <chrono>

#include "Chip8.h"

int main(int argc, char* argv[]) {
  using std::cerr;
  using std::endl;

  Chip8 chip;

  if (argc != 2) {
    std::cerr << "Invalid arguments" << std::endl;
    return EXIT_FAILURE;
  } else {
    chip.load_rom(argv[1]);
  }

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    cerr << "SDL_Init Error: " << SDL_GetError() << endl;
    return EXIT_FAILURE;
  }

  SDL_Window *win = SDL_CreateWindow("Chip8 Emulator", 0, 0, 640, 320, SDL_WINDOW_SHOWN);
  if (win == nullptr) {
    cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
    return EXIT_FAILURE;
  }

  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (ren == nullptr) {
    cerr << "SDL_CreateRenderer Error" << SDL_GetError() << endl;
    if (win != nullptr) {
      SDL_DestroyWindow(win);
    }
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_Texture *buffer = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

  bool shouldStep = false;
  auto lastCycleTime = std::chrono::high_resolution_clock::now();
  while (true) {
    if (shouldStep) {
      auto currentTime = std::chrono::high_resolution_clock::now();
      float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
      if (dt > 4) {
        lastCycleTime = currentTime;
        chip.step();
        if (SDL_UpdateTexture(buffer, NULL, &chip.video, 64 * sizeof(uint32_t)) < 0) {
          std::cout << SDL_GetError() << std::endl;
        }
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, buffer, NULL, NULL);
        SDL_RenderPresent(ren);
      }
      //shouldStep = false;
    }
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        break;
      } else if (event.type == SDL_KEYDOWN) {
        shouldStep = true;
      }
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_x: chip.keypad[0] = true; 
                       break;
          case SDLK_1: chip.keypad[1] = true; 
                       break;
          case SDLK_2: chip.keypad[2] = true; 
                       break;
          case SDLK_3: chip.keypad[3] = true; 
                       break;
          case SDLK_q: chip.keypad[4] = true; 
                       break;
          case SDLK_w: chip.keypad[5] = true; 
                       break;
          case SDLK_e: chip.keypad[6] = true; 
                       break;
          case SDLK_a: chip.keypad[7] = true; 
                       break;
          case SDLK_s: chip.keypad[8] = true; 
                       break;
          case SDLK_d: chip.keypad[9] = true; 
                       break;
          case SDLK_z: chip.keypad[10] = true; 
                       break;
          case SDLK_c: chip.keypad[11] = true; 
                       break;
          case SDLK_4: chip.keypad[12] = true; 
                       break;
          case SDLK_r: chip.keypad[13] = true; 
                       break;
          case SDLK_f: chip.keypad[14] = true; 
                       break;
          case SDLK_v: chip.keypad[15] = true; 
                       break;
        } 
      } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
          case SDLK_x: chip.keypad[0] = false; 
                       break;
          case SDLK_1: chip.keypad[1] = false; 
                       break;
          case SDLK_2: chip.keypad[2] = false; 
                       break;
          case SDLK_3: chip.keypad[3] = false; 
                       break;
          case SDLK_q: chip.keypad[4] = false; 
                       break;
          case SDLK_w: chip.keypad[5] = false; 
                       break;
          case SDLK_e: chip.keypad[6] = false; 
                       break;
          case SDLK_a: chip.keypad[7] = false; 
                       break;
          case SDLK_s: chip.keypad[8] = false; 
                       break;
          case SDLK_d: chip.keypad[9] = false; 
                       break;
          case SDLK_z: chip.keypad[10] = false; 
                       break;
          case SDLK_c: chip.keypad[11] = false; 
                       break;
          case SDLK_4: chip.keypad[12] = false; 
                       break;
          case SDLK_r: chip.keypad[13] = false; 
                       break;
          case SDLK_f: chip.keypad[14] = false; 
                       break;
          case SDLK_v: chip.keypad[15] = false; 
                       break;
        }
      }
    }
  }

  SDL_DestroyTexture(buffer);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return EXIT_SUCCESS;
}
