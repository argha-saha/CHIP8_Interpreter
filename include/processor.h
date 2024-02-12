#ifndef PROCESSOR_H
#define PROCESSOR_H

#define RAM_SIZE 4096
#define ROM_ENTRY_POINT 0x200
#define MAX_MEMORY 0xFFF
#define STACK_SIZE 16

#define RES_SCALE 1
#define WIDTH (64 * RES_SCALE)
#define HEIGHT (32 * RES_SCALE)
#define RESOLUTION (WIDTH * HEIGHT)

typedef unsigned char byte;             // 8 bits
typedef unsigned short int halfword;    // 16 bits

typedef struct {
    // Registers
    byte V_register[0x10];              // V[0] to V[F]
    halfword I_register;

    // The stack
    halfword stack_ptr;

    // Timers
    byte delay_timer;
    byte sound_timer;

    // Input

    // Graphics and sound

    // Opcode table
    halfword program_counter;
    halfword opcode;
} chip8_vm;

extern byte memory[RAM_SIZE];           // 4 KB
extern halfword stack[STACK_SIZE];
extern byte graphics[RESOLUTION];
extern byte scankey[16];

extern bool draw;

bool initialize_machine(chip8_vm *vm);
bool load_chip8_rom(sdl_t *sdl, char *rom_file);

#endif