#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "../include/chip8.h"
#include "../include/processor.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        puts("USAGE: ./chip8 <rom_file>");
        exit(EXIT_FAILURE);
    }

    sdl_t sdl = {0};
    if (initialize_sdl(&sdl) == false) {
        exit(EXIT_FAILURE);
    }

    chip8_vm vm = {0};
    if (initialize_machine(&vm) == false) {
        exit(EXIT_FAILURE);
    }

    if (load_chip8_rom(&sdl, argv[1]) == false) {
        exit(EXIT_FAILURE);
    }

    deactivate_sdl(&sdl);

    exit(EXIT_SUCCESS);
}