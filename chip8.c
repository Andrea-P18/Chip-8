#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"


void init_display(chip8*c8){
    for(int i = 0; i<DIM_DISPLAY;i++) c8->display[i] = 0x0;
}

void init_stack(chip8* c8){
    for(int i = 0; i<16;i++){
        c8 -> stack[i] = 0;
    }
}

void init_memory(chip8*c8){
    for(int i = 0; i<4096;i++){
        c8 -> memory[i] = 0;
    }
    for(int i = 0x050; i<0x050+80 ;i++){
        c8 -> memory[i] = fontset[i - 0x050];
    }
}

void init_keypad(chip8*c8){
    for(int i = 0; i<16; i++){
        c8 -> keypad[i] = 0;
    }
}


void init_chip8(chip8* c8){
    c8 -> PC = 0x200;
    init_display(c8);
    c8 -> I = 0;
    c8 -> DT = 0;
    c8 -> ST = 0;
    c8 -> SP = -1;
    init_stack(c8);
    init_keypad(c8);

}

uint8_t load_rom(char* filename, chip8* c8){

    if(!filename){
        return 1;
    }

    FILE* rom;

    if((rom = fopen(filename,"rb")) == NULL){
        fprintf(stderr,"ROM file does not exists");
        exit(2);
    }

    fseek(rom,0,SEEK_END);
    long size = ftell(rom);
    fseek(rom,0,SEEK_SET);

    fread(&c8->memory[0X200],1,size,rom); /*mettiamo tutto il file ROM in memoria*/

    fclose(rom);
    
    return 0;

}

uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void CLS(chip8* c8){
    for(int i = 0; i<DIM_DISPLAY;i++){
        c8 -> display[i] = 0;
    }
}
void RET(chip8* c8){
    if(c8 -> SP == -1){
        fprintf(stderr, "%s", "Stack vuoto, istruzione RET impossibile da eseguire");
        return;
    }
    c8 -> PC = c8 -> stack[c8 -> SP];
    c8 -> SP -= 1;
}
void JP(chip8* c8, uint16_t addr){
    c8 -> PC = addr;
}
void CALL(chip8* c8, uint16_t addr){
    c8 -> SP += 1;
    c8 -> stack[c8 -> SP] = c8 -> PC;
    c8 -> PC = addr;
}
void SE(chip8* c8, uint8_t x, uint8_t kk){
    if(x >= 16){
        fprintf(stderr,"%s", "Not enough registers (SE)");
        return;
    }
    if(c8 -> V[x] == kk)    c8 -> PC += 2;
}
void SNE(chip8* c8, uint8_t x, uint8_t kk){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers (SNE)");
        return;
    }
    if(c8 -> V[x] != kk)    c8 -> PC += 2;
}
void SEXY(chip8* c8, uint8_t x, uint8_t y){
    if(x >= 16 || y >= 16){
        fprintf(stderr,"%s","Not enough registers");
        return;
    }
    if(c8 -> V[x] == c8 -> V[y]) c8 -> PC += 2;
}
void LD(chip8* c8, uint8_t x, uint8_t kk){
    if(x >= 16){
        fprintf(stderr,"%s", "Not enough registers");
        return;
    }
    c8 -> V[x] = kk;
}
void ADD(chip8* c8, uint8_t x, uint8_t kk){
    if(x >= 16){
        fprintf(stderr,"%s", "Not enough registers");
        return;
    }
    c8 -> V[x] += kk;
}
void LDXY(chip8* c8, uint8_t x, uint8_t y){
    if(x >= 16 || y >= 16){
        fprintf(stderr,"%s", "Not enough registers");
        return;
    }
    c8 -> V[x] = c8 -> V[y];
}
void OR(chip8* c8, uint8_t x, uint8_t y){
    if(x >= 16 || y >= 16){
        fprintf(stderr,"%s", "Not enough registers");
        return;
    }
    c8 -> V[x] = c8 -> V[x] | c8 -> V[y];
}
void AND(chip8* c8, uint8_t x, uint8_t y){
    if(x >= 16 || y >= 16){
        fprintf(stderr,"%s", "Not enough registers");
        return;
    }
    c8 -> V[x] = c8 -> V[x] & c8 -> V[y];
}
void XOR(chip8* c8, uint8_t x, uint8_t y){
    if(x >= 16 || y >= 16){
        fprintf(stderr,"%s", "Not enough registers");
        return;
    }
    c8 -> V[x] = c8 -> V[x] ^ c8 -> V[y];
}
void ADDXY(chip8* c8, uint8_t x, uint8_t y){
    if(x >= 16 || y >= 16){
        fprintf(stderr,"%s", "Not enough registers");
        return;
    }
    uint16_t result = c8->V[x] + c8->V[y];
    c8->V[0xF] = (result > 0xFF) ? 1 : 0;
    c8->V[x] = (uint8_t)(result & 0xFF);
}
void SUBXY(chip8* c8, uint8_t x, uint8_t y){
    if(x >= 16 || y >= 16){
        fprintf(stderr,"%s", "Not enough registers");
        return;
    }
    c8->V[0xF] = (c8->V[x] >= c8->V[y]) ? 1 : 0;
    c8 -> V[x] = c8 -> V[x] - c8 -> V[y];
}
void SHR(chip8* c8, uint8_t x, uint8_t y){
    (void)y;
    c8->V[0xF] = c8->V[x] & 0x1;
    c8->V[x] >>= 1;
}

void SUBN(chip8* c8, uint8_t x, uint8_t y){
    if(x >= 16 || y >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    c8->V[0xF] = (c8->V[y] >= c8->V[x]) ? 1 : 0;
    c8->V[x] = c8->V[y] - c8->V[x];
}

void SHL(chip8* c8, uint8_t x, uint8_t y){
    (void)y;
    c8->V[0xF] = (c8->V[x] >> 7) & 0x1;
    c8->V[x] <<= 1;
}

void SNEXY(chip8* c8, uint8_t x, uint8_t y){
    if(x >= 16 || y >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    if(c8->V[x] != c8->V[y]) c8->PC += 2;
}

void LD_I(chip8* c8, uint16_t addr){
    c8->I = addr;
}

void JP_V0(chip8* c8, uint16_t addr){
    c8->PC = c8->V[0x0] + addr;
}

void RND(chip8* c8, uint8_t x, uint8_t kk){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    c8->V[x] = (rand() % 256) & kk;
}

void DRW(chip8* c8, uint8_t x, uint8_t y, uint8_t n){
    uint8_t xpos = c8->V[x] % 64;
    uint8_t ypos = c8->V[y] % 32;
    c8->V[0xF] = 0;

    for(int row = 0; row < n; row++){
        uint8_t sprite_byte = c8->memory[c8->I + row];
        for(int col = 0; col < 8; col++){
            uint8_t sprite_pixel = (sprite_byte >> (7 - col)) & 0x1;
            int px = (xpos + col) % 64;
            int py = (ypos + row) % 32;
            int idx = py * 64 + px;
            if(sprite_pixel){
                if(c8->display[idx]) c8->V[0xF] = 1;
                c8->display[idx] ^= 1;
            }
        }
    }
}

void SKP(chip8* c8, uint8_t x){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    if(c8->keypad[c8->V[x]]) c8->PC += 2;
}

void SKNP(chip8* c8, uint8_t x){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    if(!c8->keypad[c8->V[x]]) c8->PC += 2;
}

void LD_Vx_DT(chip8* c8, uint8_t x){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    c8->V[x] = c8->DT;
}

void LD_Vx_K(chip8* c8, uint8_t x){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    for(int i = 0; i < 16; i++){
        if(c8->keypad[i]){
            c8->V[x] = i;
            return;
        }
    }
    c8->PC -= 2; // Nessun tasto premuto: ripete l'istruzione
}

void LD_DT_Vx(chip8* c8, uint8_t x){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    c8->DT = c8->V[x];
}

void LD_ST_Vx(chip8* c8, uint8_t x){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    c8->ST = c8->V[x];
}

void ADD_I_Vx(chip8* c8, uint8_t x){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    c8->I += c8->V[x];
}

void LD_F_Vx(chip8* c8, uint8_t x){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    c8->I = 0x050 + (c8->V[x] * 5);
}

void LD_B_Vx(chip8* c8, uint8_t x){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    uint8_t val = c8->V[x];
    c8->memory[c8->I]     = val / 100;
    c8->memory[c8->I + 1] = (val / 10) % 10;
    c8->memory[c8->I + 2] = val % 10;
}

void STR_V(chip8* c8, uint8_t x){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    for(int i = 0; i <= x; i++){
        c8->memory[c8->I + i] = c8->V[i];
    }
}

void LDR_V(chip8* c8, uint8_t x){
    if(x >= 16){
        fprintf(stderr, "%s", "Not enough registers");
        return;
    }
    for(int i = 0; i <= x; i++){
        c8->V[i] = c8->memory[c8->I + i];
    }
}