#ifndef CHIP8_H
#define CHIP8_H

typedef struct {
    unsigned char memory[4096];		// memory 4kb
    unsigned char V[16];		// registers V0-VF
    unsigned short stack[16];		// store the addresses to return when finished a subroutine
    unsigned char screen[64 * 32];	// screen is 64 * 32 2048pixels. graphics are black and white
    unsigned char sprites[80];		// sprites from 0-9, A-F
    unsigned char keypad[16];		// 16 key hex keypad
    unsigned short PC;			// program counter. store currently executing address
    unsigned short I;			// index register. point at locations in memory
    unsigned short SP;			// stack pointer. point to the topmost level of the stack
    unsigned char delay_timer;		// decrements at 60hz if value is > 0
    unsigned char sound_timer;		// decrements at 60hz if value is > 0
    unsigned short opcode;		// instruction to execute. most significant byte first
}chip8_t;

void initialize(chip8_t *chip8);

void emulateCycle(chip8_t*);

//void emulateCycle();
//
//void loadGame(char arg);
//
//void setupGraphics();
//
//void setupInput();
//
//void setKeys();
//
//void drawGraphics();

#endif
