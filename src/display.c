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
    int delay = 16;

    while (quit == false) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    deactivate_sdl(sdl);
                    quit = true;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;

                        case SDLK_F1:
                            initialize_machine(vm);
                            load_chip8_rom(sdl, rom_file);
                            cpu_cycle(sdl, vm);
                            update_display(sdl, vm, rom_file);
                            deactivate_sdl(sdl);
                            break;

                        case SDLK_F2:
                            delay--;
                            break;

                        case SDLK_F3:
                            delay++;
                            break;

                        case SDLK_p:
                            paused = !paused;
                            break;

                        case SDLK_n:
                            initialize_machine(vm);
                            update_display(sdl, vm, rom_file);
                            break;

                        case SDLK_x:
                            scankey[0] = 1;
                            break;

                        case SDLK_1:
                            scankey[1] = 1;
                            break;

                        case SDLK_2:
                            scankey[2] = 1;
                            break;

                        case SDLK_3:
                            scankey[3] = 1;
                            break;

                        case SDLK_q:
                            scankey[4] = 1;
                            break;

                        case SDLK_w:
                            scankey[5] = 1;
                            break;

                        case SDLK_e:
                            scankey[6] = 1;
                            break;

                        case SDLK_a:
                            scankey[7] = 1;
                            break;

                        case SDLK_s:
                            scankey[8] = 1;
                            break;

                        case SDLK_d:
                            scankey[9] = 1;
                            break;

                        case SDLK_z:
                            scankey[0xA] = 1;
                            break;

                        case SDLK_c:
                            scankey[0xB] = 1;
                            break;

                        case SDLK_4:
                            scankey[0xC] = 1;
                            break;

                        case SDLK_r:
                            scankey[0xD] = 1;
                            break;

                        case SDLK_f:
                            scankey[0xE] = 1;
                            break;

                        case SDLK_v:
                            scankey[0xF] = 1;
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_x:
                            scankey[0] = 0;
                            break;

                        case SDLK_1:
                            scankey[1] = 0;
                            break;

                        case SDLK_2:
                            scankey[2] = 0;
                            break;

                        case SDLK_3:
                            scankey[3] = 0;
                            break;

                        case SDLK_q:
                            scankey[4] = 0;
                            break;

                        case SDLK_w:
                            scankey[5] = 0;
                            break;

                        case SDLK_e:
                            scankey[6] = 0;
                            break;

                        case SDLK_a:
                            scankey[7] = 0;
                            break;

                        case SDLK_s:
                            scankey[8] = 0;
                            break;

                        case SDLK_d:
                            scankey[9] = 0;
                            break;

                        case SDLK_z:
                            scankey[0xA] = 0;
                            break;

                        case SDLK_c:
                            scankey[0xB] = 0;
                            break;

                        case SDLK_4:
                            scankey[0xC] = 0;
                            break;

                        case SDLK_r:
                            scankey[0xD] = 0;
                            break;

                        case SDLK_f:
                            scankey[0xE] = 0;
                            break;

                        case SDLK_v:
                            scankey[0xF] = 0;
                            break;
                    }
                    break;
            }
            break;
        }

        if (delay < 0) {
            delay = 0;
        } else {
            SDL_Delay(delay);
        }

        if (vm -> delay_timer > 0) {
            --vm -> delay_timer;
        }

        if (paused == false) {
            cpu_cycle(sdl, vm);
        }

        if (draw == true) {
            draw_graphics(sdl);
        }
    }
}

void draw_graphics(sdl_t *sdl) {
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 1;
    rect.h = 1;

    SDL_SetRenderDrawColor(sdl -> renderer, 0, 0, 0, 255);
    SDL_RenderClear(sdl -> renderer);
    SDL_SetRenderDrawColor(sdl -> renderer, 0, 255, 0, 0);

    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            if (graphics[x + (y * WIDTH)] == 1) {
                rect.x = x;
                rect.y = y;
                SDL_RenderFillRect(sdl -> renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(sdl -> renderer);
    draw = false;
}