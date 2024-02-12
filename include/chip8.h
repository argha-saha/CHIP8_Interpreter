#ifndef CHIP8_H
#define CHIP8_H

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} sdl_t;

bool initialize_sdl(sdl_t *sdl);
void deactivate_sdl(sdl_t *sdl);

#endif