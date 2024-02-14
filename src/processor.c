#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <time.h>

#include "../include/chip8.h"

byte memory[RAM_SIZE];
halfword stack[STACK_SIZE];
word graphics[RESOLUTION];
byte scankey[16];

bool draw;

bool initialize_machine(chip8_vm *vm) {
    vm -> program_counter = ROM_ENTRY_POINT;
    draw = true;

    // Reset memory
    memset(memory, 0, MAX_MEMORY * sizeof(byte));
    memset(stack, 0, STACK_SIZE * sizeof(halfword));
    memset(graphics, 0, RESOLUTION * sizeof(word));
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

void cpu_cycle(chip8_vm *vm) {
	vm -> opcode = memory[vm -> program_counter] << 8 | memory[vm -> program_counter + 1];
	vm -> program_counter +=2;

	byte X       = (vm -> opcode & 0x0F00) >> 8;
	byte Y       = (vm -> opcode & 0x00F0) >> 4;
	halfword NNN = (vm -> opcode & 0x0FFF);
	byte NN      = (vm -> opcode & 0x00FF);
	byte N       = (vm -> opcode & 0x000F);
    bool key_pressed;

    printf("============\n");
	printf("OPCODE: %x \n", vm -> opcode);
	printf("PC: %x \n", vm -> program_counter);
	printf("I Reg: %x \n", vm -> I_register);
	
	switch (vm -> opcode & 0xF000) {
        // 00xx
        case 0x0000:
            switch (vm -> opcode & 0x00FF) {
                // 00E0: Clears the screen
                // C Pseudo: disp_clear()
                case 0x00E0:
                    memset(graphics, 0, 2048 * sizeof(word));
                    draw = true;
                    break;

                // 00EE: Returns from a subroutine
                // C Pseudo: return;
                case 0x00EE:
                    --vm -> stack_ptr;
                    vm -> program_counter = stack[vm -> stack_ptr];
                    break;
            }
            break;

        // 1NNN: Jumps to address NNN
        // C Pseudo: goto NNN;
        case 0x1000:
            vm -> program_counter = NNN;
            break;
        
        // 2NNN: Calls subroutine at NNN
        // C Pseudo: *(0xNNN)()
        case 0x2000:
            stack[vm -> stack_ptr] = vm -> program_counter;
            ++vm -> stack_ptr;
            vm -> program_counter = NNN;
            break;
        
        // 3XNN: Skips the next instruction if VX equals NN
        // C Pseudo: if (VX == NN)
        case 0x3000: 
            if (vm -> V_register[X] == NN) {
                vm -> program_counter += 2;
            }
            break;
        
        // 4XNN: Skips the next instruction if VX does not equal NN
        // C Pseudo: if (VX != NN)
        case 0x4000:
            if (vm -> V_register[X] != NN) {
                vm -> program_counter += 2;
            }
            break;

        // 5XY0: Skips the next instruction if VX equals VY
        // C Pseudo: if (VX == VY)
        case 0x5000:
            if (vm -> V_register[X] == vm -> V_register[Y]) {
                vm -> program_counter += 2;
            }
            break;
        
        // 6XNN: Sets VX to NN
        // C Pseudo: VX = NN
        case 0x6000:
            vm -> V_register[X] = NN;
            break;
        
        // 7XNN: Adds NN to VX
        // C Pseudo: VX += NN
        case 0x7000:
            vm -> V_register[(X)] += NN;
            break;

        // 8XYN
        case 0x8000:
            switch(N){
                // 8XY0: Sets VX to the value of VY
                // C Pseudo: VX = VY
                case 0x0000:
                    vm -> V_register[X] = vm -> V_register[Y];
                    break;

                // 8XY1: Sets VX to VX or VY
                // C Pseudo: VX |= VY
                case 0x0001:
                    vm -> V_register[X] |= vm -> V_register[Y];
                    break;

                // 8XY2: Sets VX to VX and VY
                // C Pseudo: VX &= VY
                case 0x0002:
                    vm -> V_register[X] &= vm -> V_register[Y];
                    break;

                // 8XY3: Sets VX to VX xor VY
                // C Pseudo: VX ^= VY
                case 0x0003:
                    vm -> V_register[X] ^= vm -> V_register[Y];
                    break;

                // 8XY4: Adds VY to VX
                // VF is set to 1 when there's an overflow, and to 0 when there is not
                // C Pseudo: VX += VY
                case 0x0004:
                    int result = (int)(vm -> V_register[X]) + (int)(vm -> V_register[Y]);

                    if (result > 255) {
                        vm -> V_register[0xF] = 1;
                    } else {
                        vm -> V_register[0xF] = 0;
                    }

                    vm -> V_register[X] = result & 0xFF;

                    break;

                // 8XY5: VY is subtracted from VX
                // VF is set to 0 when there's an underflow, and 1 when there is not
                // C Pseudo: VX -= VY
                case 0x0005:
                    if ((vm -> V_register[X]) > (vm -> V_register[Y])) {
                        vm -> V_register[0xF] = 1;
                    } else {
                        vm -> V_register[0xF] = 0;
                    }

                    vm -> V_register[X] -= vm -> V_register[Y];

                    break;

                // 8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1
                // C Pseudo: VX >>= 1
                case 0x0006:
                    vm -> V_register[0xF] = vm -> V_register[X] & 1;
                    vm -> V_register[X] >>= 1;
                    break;

                // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not
                // C Pseudo: VX = VY - VX
                case 0x0007:
                    if(vm -> V_register[Y] > vm -> V_register[X]) {
                        vm -> V_register[0xF] = 1;
                    } else {
                        vm -> V_register[0xF] = 0;
                    }

                    vm -> V_register[X] = (vm -> V_register[Y]) - (vm -> V_register[X]);

                    break;

                // 8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1
                // C Pseudo: VX <<= 1
                case 0x000E:
                    vm -> V_register[0xF] = vm -> V_register[X] >> 7;
                    vm -> V_register[X] <<= 1;
                    break;
            }
            break;
        
        // 9XY0: Skips the next instruction if VX does not equal VY
        // C Pseudo: if (VX != VY)
        case 0x9000:
            if ((vm -> V_register[X]) != (vm -> V_register[Y])) {
                vm -> program_counter += 2;
            }

            break;

        // ANNN: Sets I to the address NNN
        // C Pseudo: I = NNN
        case 0xA000:
            vm -> I_register = NNN;
            break;

        // BNNN: Jumps to the address NNN plus V0
        // C Pseudo: PC = V0 + NNN
        case 0xB000:
            vm -> program_counter = (vm -> V_register[0x0]) + NNN;
            break;

        // CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN
        // C Pseudo: VX = rand() & NN
        case 0xC000:
            vm -> V_register[X] = (rand() % 0x100) & NN;
            break;

        // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels
        // Each row of 8 pixels is read as bit-coded starting from memory location I
        // I value does not change after the execution of this instruction
        // As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, 
        // and to 0 if that does not happen
        // C Pseudo: draw(VX, VY, N)
        case 0xD000:;
            halfword x = vm -> V_register[X];
            halfword y = vm -> V_register[Y];
            halfword height = N;
            byte pixel;

            vm -> V_register[0xF] = 0;

            for (int y_line = 0; y_line < height; ++y_line) {
                pixel = memory[vm -> I_register + y_line];
                
                for(int x_line = 0; x_line < 8; ++x_line) {
                    if((pixel & (0x80 >> x_line)) != 0) {
                        if(graphics[(x + x_line + ((y + y_line) * WIDTH))] == 1){
                            vm -> V_register[0xF] = 1;                                   
                        }

                        graphics[x + x_line + ((y + y_line) * WIDTH)] ^= 1;
                    }
                }
            }

            draw = true;
            break;
        
        // EXxx
        case 0xE000:
            switch(NN){
                // EX9E: Skips the next instruction if the key stored in VX is pressed
                // C Pseudo: if (key() == VX)
                case 0x009E:
                    if(scankey[vm -> V_register[X]] != 0) {
                        vm -> program_counter += 2;
                    }
                    break;	

                // EXA1: Skips the next instruction if the key stored in VX is not pressed
                // C Pseudo: if (key() != VX)
                case 0x00A1:
                    if(scankey[vm -> V_register[X]] == 0) {
                        vm -> program_counter += 2;
                    }
                    break;
            }
            break;

        // FXxx
        case 0xF000:
            switch (NN) {
                // FX07: Sets VX to the value of the delay timer
                // C Pseudo: VX = get_delay()
                case 0x0007:
                    vm -> V_register[X] = vm -> delay_timer;
                    break;

                // FX0A: A key press is awaited, and then stored in VX
                // C Pseudo: VX = get_key()
                case 0x000A:
                    key_pressed = false;

                    for (word i = 0; i < 16; ++i) {
                        if (scankey[i]) {
                            key_pressed = true;
                            vm -> V_register[X] = i;
                        }
                    }

                    if (key_pressed == false) {
                        vm -> program_counter -= 2;
                    }
                    
                    break;

                
            }
            break;
    }
}