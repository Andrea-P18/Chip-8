#include <stdio.h>
#include <stdint.h>

typedef struct{

    uint8_t memory[4096]; /*Memoria di 4096 byte*/
    uint8_t V[16];       /*Registri di memoria chiamati Vx per x da 0 ad F*/
    uint16_t I;          /*Registro di memoria contenente indirizzi di memoria*/

    
}chip8;