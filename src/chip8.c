#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "../include/chip8.h"

bool initialize_sdl(sdl_t *sdl) {
    // Initialize SDL Subsystems
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Create SDL Window
    sdl -> window = SDL_CreateWindow("CHIP-8 Interpreter",
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     800,
                                     400,
                                     0);

    if (sdl -> window == NULL) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        return false;
    }

    // Create SDL Renderer
    sdl -> renderer = SDL_CreateRenderer(sdl -> window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (sdl -> renderer == NULL) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        return false;
    }

    // Original Resolution: 64x32
    SDL_RenderSetLogicalSize(sdl -> renderer, 64, 32);
    SDL_SetRenderDrawColor(sdl -> renderer, 0, 0, 0, 255);

    return true;
}

void deactivate_sdl(sdl_t *sdl) {
    SDL_DestroyTexture(sdl -> texture);
    SDL_DestroyRenderer(sdl -> renderer);
    SDL_DestroyWindow(sdl -> window);
    SDL_Quit();
}
