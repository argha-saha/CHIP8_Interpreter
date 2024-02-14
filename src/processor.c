#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "../include/chip8.h"

byte memory[RAM_SIZE];
halfword stack[STACK_SIZE];
byte graphics[RESOLUTION];
byte scankey[16];

bool draw;

bool initialize_machine(chip8_vm *vm) {
    vm -> program_counter = ROM_ENTRY_POINT;
    draw = true;

    // Reset memory
    memset(memory, 0, MAX_MEMORY * sizeof(byte));
    memset(stack, 0, STACK_SIZE * sizeof(halfword));
    memset(graphics, 0, RESOLUTION * sizeof(byte));
    memset(scankey, 0, 16);
    memset(vm -> V_register, 0, 16);

    // Left to right, top to bottom
    /* 
    EXAMPLE: #1
    00100000
    01100000
    00100000
    00100000
    01110000
    */
    byte font_set[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
        0x20, 0x60, 0x20, 0x20, 0x70,   // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
        0xF0, 0x80, 0xF0, 0x80, 0x80,   // F
    };

    // Font starts at 0x50
    for (int i = 0; i < 80; ++i) {
        memory[0x50 + i] = font_set[i];
    }

    return true;
}

bool load_chip8_rom(sdl_t *sdl, char *rom_file) {
    SDL_RenderClear(sdl -> renderer);
    sdl -> texture = SDL_CreateTexture(sdl -> renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    FILE *rom = fopen(rom_file, "rb");
    if (rom == NULL) {
        SDL_Log("Could not open ROM file: %s", rom_file);
        return false;
    }

    // Calculate ROM size
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    // Store ROM in buffer
    char *buffer = (char *) malloc(rom_size * sizeof(char));
    if (buffer == NULL) {
        SDL_Log("Could not store ROM in buffer: %s", rom_file);
        return false;
    }

    // Copy ROM file into buffer
    size_t res = fread(buffer, 1, rom_size, rom);
    if ((long) res != rom_size) {
        SDL_Log("Could not copy ROM file %s into buffer", rom_file);
        return false;
    }

    // Copy buffer to memory
    if (rom_size < (0xfff - 0x200)) {
        for (int i = 0; i < rom_size; ++i) {
            memory[i + 0x200] = buffer[i];
        }
    } else {
        SDL_Log("ROM file %s is too large", rom_file);
        return false;
    }

    fclose(rom);
    free(buffer);

    return true;
}

void cpu_cycle(sdl_t *sdl, chip8_vm *vm) {
    (void) *sdl;
    (void) *vm;

    

    // Execute instructions
    
}