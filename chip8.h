#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // Sostituito stdvoid.h

typedef struct{
    //memory section
    uint8_t memory[4096]; /*Memoria di 4096 byte*/
    
    //registers section
    uint8_t V[16];       /*Registri di memoria chiamati Vx per x da 0 ad F*/
    uint16_t I;          /*Registro di memoria contenente indirizzi di memoria*/
    uint8_t DT;          /*Delay timer register*/
    uint8_t ST;          /*Sound timer register*/
    uint16_t PC;
    uint8_t SP;
    
    //stack section
    uint16_t stack[16];

    //Keyboard section
    uint8_t keypad[16];

    //Display
    uint8_t display[64*32]; /*I valori all'interno saranno 0 o 1*/

} chip8;

// --- Istruzioni che avevi già scritto (corrette) ---
void SYS(chip8* c8, uint16_t addr); 
void CLS(chip8* c8); 
void RET(chip8* c8); 
void JP(chip8* c8, uint16_t addr); 
void CALL(chip8* c8, uint16_t addr); 
void SE(chip8* c8, uint8_t x, uint8_t kk); // Aggiunto il punto e virgola
void SNE(chip8* c8, uint8_t x, uint8_t kk); 
void SEXY(chip8* c8, uint8_t x, uint8_t y); 
void LD(chip8* c8, uint8_t x, uint8_t kk); 
void ADD(chip8* c8, uint8_t x, uint8_t kk); 
void LDXY(chip8* c8, uint8_t x, uint8_t y); 
void OR(chip8* c8, uint8_t x, uint8_t y);
void AND(chip8* c8, uint8_t x, uint8_t y);
void XOR(chip8* c8, uint8_t x, uint8_t y);
void ADDXY(chip8* c8, uint8_t x, uint8_t y);
void SUBXY(chip8* c8, uint8_t x, uint8_t y);
void SHR(chip8* c8, uint8_t x, uint8_t y);
void SUBN(chip8* c8, uint8_t x, uint8_t y);
void SHL(chip8* c8, uint8_t x, uint8_t y);
void SNEXY(chip8* c8, uint8_t x, uint8_t y);
void LD_I(chip8* c8, uint16_t addr);
void JP_V0(chip8* c8, uint16_t addr);
void RND(chip8* c8, uint8_t x, uint8_t kk);
void DRW(chip8* c8, uint8_t x, uint8_t y, uint8_t nibble);
void SKP(chip8* c8, uint8_t x);
void SKNP(chip8* c8, uint8_t x);
void LD_Vx_DT(chip8* c8, uint8_t x);  /* Fx07 - Imposta Vx uguale al valore del Delay Timer */
void LD_Vx_K(chip8* c8, uint8_t x);   /* Fx0A - Attende la pressione di un tasto e lo salva in Vx */
void LD_DT_Vx(chip8* c8, uint8_t x);  /* Fx15 - Imposta il Delay Timer uguale a Vx */
void LD_ST_Vx(chip8* c8, uint8_t x);  /* Fx18 - Imposta il Sound Timer uguale a Vx */
void ADD_I_Vx(chip8* c8, uint8_t x);  /* Fx1E - Somma Vx al registro I (I = I + Vx) */
void LD_F_Vx(chip8* c8, uint8_t x);   /* Fx29 - Imposta I all'indirizzo del font corrispondente al carattere in Vx */
void LD_B_Vx(chip8* c8, uint8_t x);   /* Fx33 - Salva la rappresentazione BCD di Vx in I, I+1 e I+2 */
void STR_V(chip8* c8, uint8_t x);     /* Fx55 - Salva in memoria i registri da V0 a Vx partendo dall'indirizzo I */
void LDR_V(chip8* c8, uint8_t x);     /* Fx65 - Carica dalla memoria i registri da V0 a Vx partendo dall'indirizzo I */