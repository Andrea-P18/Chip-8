#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){

    chip8 c8;
    init_chip8(&c8);

    load_rom("file", &c8);

    uint16_t instruction = 0;
    uint8_t  nibble;
    uint8_t  x;
    uint8_t  y;
    uint16_t nnn;
    uint8_t  kk;
    uint8_t  n;

    while(1){
        if(c8.PC >= 4096){ fprintf(stderr, "PC out of bounds\n"); break; }

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

    return 0;
}