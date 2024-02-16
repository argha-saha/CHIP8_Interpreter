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
typedef unsigned long word;             // 32 bits

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} sdl_t;

typedef struct {
    // Memory
    byte memory[RAM_SIZE];       // 4 KB RAM

    // Registers
    byte V_register[0x10];       // V[0] to V[F]
    halfword I_register;         // 12-bit index register

    // The stack
    halfword stack[STACK_SIZE];  // Stores 16-bit addresses
    halfword stack_ptr;          // Stores top of stack

    // Timers (count down at 60hz until they reach 0)
    byte delay_timer;            // Used for timing events
    byte sound_timer;            // Beeping sound is made if value is nonzero

    // Input
    byte scankey[16];            // 16 keys ranging 0 to F

    // Graphics and sound
    word graphics[RESOLUTION];   // 64x32 original resolution

    // Opcode table
    halfword program_counter;    // Points at current instruction in memory
    halfword opcode;             // Each opcode is 2 bytes long and stored big-endian
} chip8_vm;

extern bool draw;

bool initialize_sdl(sdl_t *sdl);
void deactivate_sdl(sdl_t *sdl);

bool initialize_machine(chip8_vm *vm);
bool load_chip8_rom(sdl_t *sdl, chip8_vm *vm, char *rom_file);
void cpu_cycle(chip8_vm *vm);

#endif