#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "../include/processor.h"

bool initialize_machine(chip8_vm vm) {
    vm.program_counter = ROM_ENTRY_POINT;
    draw = true;

    // Reset memory
    memset(memory, 0, MAX_MEMORY);
    memset(stack, 0, STACK_SIZE);
    memset(graphics, 0, RESOLUTION);
    memset(scankey, 0, 16);
    memset(vm.V_register, 0, 16);
}