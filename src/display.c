#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "../include/chip8.h"
#include "../include/display.h"

bool paused = false;
bool quit = false;
SDL_Event event;

void update_display(sdl_t *sdl, chip8_vm *vm, char *rom_file) {
    (void) *vm;
    (void) *rom_file;

    // int delay = 16;

    while (quit == false) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    deactivate_sdl(sdl);
                    quit = true;
                    break;
                
                default:
                    break;
            }
        }
    }
}