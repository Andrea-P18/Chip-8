#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"
#include "fontset.c"
#include "load_rom.c"

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
        c8 -> memory[i] = fontset[i];
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
    c8 -> SP = 0;
    init_stack(c8);
    init_keypad(c8);
}



int main(int argc, char**argv){

    chip8 c8;
    init_chip8(&c8);

    

}