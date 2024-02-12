#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "../include/processor.h"

byte memory[RAM_SIZE];                  // 4 KB
halfword stack[STACK_SIZE];
byte graphics[RESOLUTION];
byte scankey[16];

bool draw;

bool initialize_machine(chip8_vm vm) {
    vm.program_counter = ROM_ENTRY_POINT;
    draw = true;

    // Reset memory
    memset(memory, 0, MAX_MEMORY * sizeof(byte));
    memset(stack, 0, STACK_SIZE * sizeof(halfword));
    memset(graphics, 0, RESOLUTION * sizeof(byte));
    memset(scankey, 0, 16);
    memset(vm.V_register, 0, 16);

    return true;
}