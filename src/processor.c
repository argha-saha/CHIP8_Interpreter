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

    vm -> opcode = memory[vm -> program_counter] << 8 | memory[vm -> program_counter + 1];
    vm -> program_counter += 2;

    // Execute instructions
    halfword bit0 = 0x000F;
    halfword bit1 = 0x00F0;
    halfword bit2 = 0x0F00;
    halfword bit3 = 0xF000;
    halfword NNN  = 0x0FFF;
    halfword NN   = 0x00FF;
    halfword N    = 0x000F;

    halfword XY   = 0x0FF0;
    byte X = (vm -> opcode & bit2) >> 8;
    byte Y = (vm -> opcode & bit1) >> 4;

    switch (vm -> opcode & bit3) {
        case 0x0000:
            switch (vm -> opcode & bit0) {
                // 00E0: Clears the screen | C Pseudo: disp_clear()
                case 0x0000:
                    memset(graphics, 0, RESOLUTION);
                    draw = true;
                    break;

                // 00EE: Returns from a subroutine | C Pseudo: return;
                case 0x000E:
                    --vm -> stack_ptr;
                    vm -> program_counter = stack[vm -> program_counter];
                    break;

                default:
                    SDL_Log("OPCODE Error: 0xxx -> %x", vm -> opcode);
                    vm -> program_counter = vm -> opcode & NNN;
            }
            break;
        
        // 1NNN: Jumps to address NNN | C Pseudo: goto NNN;
        case 0x1000:
            vm -> program_counter = vm -> opcode & NNN;
            break;

        // 2NNN: Calls subroutine at NNN | C Pseudo: *(0xNNN)()
        case 0x2000:
            stack[vm -> stack_ptr] = vm -> program_counter;     // Save return addr in stack
            vm -> stack_ptr++;
            vm -> program_counter = vm -> opcode & NNN;
            break;

        // 3XNN: Skips the next instruction if VX == NN | C Pseudo: if (VX == NN)
        case 0x3000:
            if (vm -> V_register[X] == (vm -> opcode & NN)) {
                vm -> program_counter += 2;
            }
            break;

        // 4XNN: Skips the next instruction if VX != NN | C Pseudo: if (VX != NN)
        case 0x4000:
            if (vm -> V_register[X] != (vm -> opcode & NN)) {
                vm -> program_counter += 2;
            }
            break;

        // 5XY0: Skips the next instruction if VX equals VY | C Pseudo: if (VX == VY)
        case 0x5000:
            if (vm -> V_register[X] == vm -> V_register[Y]) {
                vm -> program_counter += 2;
            }
            break;

        // 6XNN: Sets VX to NN | C Pseudo: VX == NN
        case 0x6000:
            vm -> V_register[X] = vm -> opcode & NN;
            break;

        // 7XNN: Adds NN to VX | C Pseudo: VX += NN
        case 0x7000:
            vm -> V_register[X] += vm -> opcode & NN;
            break;

        
    }
}