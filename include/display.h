#ifndef DISPLAY_H
#define DISPLAY_H

#include "../include/chip8.h"

void update_display(sdl_t *sdl, chip8_vm *vm, char *rom_file);
void draw_graphics(sdl_t *sdl, chip8_vm *vm);

#endif