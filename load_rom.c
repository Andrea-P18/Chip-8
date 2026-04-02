#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "chip8.h"


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