#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define SCALE       10        // ogni pixel CHIP-8 = 10x10 pixel reali
#define WINDOW_W    64 * SCALE
#define WINDOW_H    32 * SCALE
#define FPS         60
#define IPS         700       // istruzioni per secondo

// Mappa tasti SDL → tasti CHIP-8
// Layout CHIP-8:    Layout tastiera:
// 1 2 3 C          1 2 3 4
// 4 5 6 D          Q W E R
// 7 8 9 E          A S D F
// A 0 B F          Z X C V
int keymap[16] = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3,
    SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c,
    SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

void draw(SDL_Renderer* renderer, chip8* c8){
    // Sfondo nero
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Pixel accesi in bianco
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(int i = 0; i < DIM_DISPLAY; i++){
        if(c8->display[i]){
            int px = (i % 64) * SCALE;
            int py = (i / 64) * SCALE;
            SDL_Rect rect = { px, py, SCALE, SCALE };
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char** argv){

    if(argc < 2){
        fprintf(stderr, "Uso: %s <rom.ch8>\n", argv[0]);
        return 1;
    }

    // Init SDL
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H, 0
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Init CHIP-8
    chip8 c8;
    init_chip8(&c8);
    load_rom(argv[1], &c8);

    uint16_t instruction = 0;
    uint8_t  x, y, n;
    uint16_t nnn;
    uint8_t  kk;

    int running = 1;
    Uint32 last_timer = SDL_GetTicks();
    Uint32 last_frame = SDL_GetTicks();
    int instructions_per_frame = IPS / FPS;

    while(running){

        // --- Input ---
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) running = 0;

            if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP){
                int pressed = (event.type == SDL_KEYDOWN) ? 1 : 0;
                for(int i = 0; i < 16; i++){
                    if(event.key.keysym.sym == keymap[i]){
                        c8.keypad[i] = pressed;
                    }
                }
                if(event.key.keysym.sym == SDLK_ESCAPE) running = 0;
            }
        }

        // --- Esegui istruzioni ---
        for(int i = 0; i < instructions_per_frame; i++){
            if(c8.PC >= 4096){ fprintf(stderr, "PC out of bounds\n"); running = 0; break; }

            instruction = c8.memory[c8.PC] << 8 | c8.memory[c8.PC + 1];
            c8.PC += 2;

            n   =  instruction & 0x000F;
            y   = (instruction & 0x00F0) >> 4;
            x   = (instruction & 0x0F00) >> 8;
            nnn =  instruction & 0x0FFF;
            kk  =  instruction & 0x00FF;

            switch(instruction & 0xF000){
                case 0x0000:
                    switch(kk){
                        case 0x00E0: CLS(&c8);  break;
                        case 0x00EE: RET(&c8);  break;
                    }
                    break;
                case 0x1000: JP(&c8, nnn);          break;
                case 0x2000: CALL(&c8, nnn);        break;
                case 0x3000: SE(&c8, x, kk);        break;
                case 0x4000: SNE(&c8, x, kk);       break;
                case 0x5000: SEXY(&c8, x, y);       break;
                case 0x6000: LD(&c8, x, kk);        break;
                case 0x7000: ADD(&c8, x, kk);       break;
                case 0x8000:
                    switch(n){
                        case 0x0: LDXY(&c8, x, y);  break;
                        case 0x1: OR(&c8, x, y);    break;
                        case 0x2: AND(&c8, x, y);   break;
                        case 0x3: XOR(&c8, x, y);   break;
                        case 0x4: ADDXY(&c8, x, y); break;
                        case 0x5: SUBXY(&c8, x, y); break;
                        case 0x6: SHR(&c8, x, y);   break;
                        case 0x7: SUBN(&c8, x, y);  break;
                        case 0xE: SHL(&c8, x, y);   break;
                    }
                    break;
                case 0x9000: SNEXY(&c8, x, y);      break;
                case 0xA000: LD_I(&c8, nnn);        break;
                case 0xB000: JP_V0(&c8, nnn);       break;
                case 0xC000: RND(&c8, x, kk);       break;
                case 0xD000: DRW(&c8, x, y, n);     break;
                case 0xE000:
                    switch(kk){
                        case 0x9E: SKP(&c8, x);     break;
                        case 0xA1: SKNP(&c8, x);    break;
                    }
                    break;
                case 0xF000:
                    switch(kk){
                        case 0x07: LD_Vx_DT(&c8, x); break;
                        case 0x0A: LD_Vx_K(&c8, x);  break;
                        case 0x15: LD_DT_Vx(&c8, x); break;
                        case 0x18: LD_ST_Vx(&c8, x); break;
                        case 0x1E: ADD_I_Vx(&c8, x); break;
                        case 0x29: LD_F_Vx(&c8, x);  break;
                        case 0x33: LD_B_Vx(&c8, x);  break;
                        case 0x55: STR_V(&c8, x);     break;
                        case 0x65: LDR_V(&c8, x);     break;
                    }
                    break;
                default:
                    fprintf(stderr, "Opcode sconosciuto: 0x%04X\n", instruction);
                    break;
            }
        }

        // --- Timer a 60Hz ---
        Uint32 now = SDL_GetTicks();
        if(now - last_timer >= 1000 / FPS){
            if(c8.DT > 0) c8.DT--;
            if(c8.ST > 0) c8.ST--;
            last_timer = now;
        }

        // --- Rendering ---
        draw(renderer, &c8);

        // --- Cap a 60 FPS ---
        Uint32 elapsed = SDL_GetTicks() - last_frame;
        if(elapsed < 1000 / FPS) SDL_Delay(1000 / FPS - elapsed);
        last_frame = SDL_GetTicks();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}