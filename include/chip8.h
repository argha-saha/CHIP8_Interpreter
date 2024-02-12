#ifndef CHIP8_H
#define CHIP8_H

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

bool intialize_sdl(sdl_t *sdl);
void deactivate_sdl(sdl_t *sdl);

#endif