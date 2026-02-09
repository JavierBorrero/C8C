#include <string.h>
#include <stdio.h>
#include "chip8.h"

unsigned char sprites[80] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xf0, 0x10, 0xf0, 0x80, 0xf0, // 2
    0xf0, 0x10, 0xf0, 0x10, 0xf0, // 3
    0x90, 0x90, 0xf0, 0x10, 0x10, // 4
    0xf0, 0x80, 0xf0, 0x10, 0x40, // 5
    0xf0, 0x80, 0xf0, 0x90, 0xf0, // 6
    0xf0, 0x10, 0x20, 0x40, 0x40, // 7
    0xf0, 0x90, 0xf0, 0x90, 0xf0, // 8
    0xf0, 0x90, 0xf0, 0x10, 0xf0, // 9
    0xf0, 0x90, 0xf0, 0x90, 0x90, // A
    0xe0, 0x90, 0xe0, 0x90, 0xe0, // B
    0xf0, 0x80, 0x80, 0x80, 0xf0, // C
    0xe0, 0x90, 0x90, 0x90, 0xe0, // D
    0xf0, 0x80, 0xf0, 0x80, 0xf0, // E
    0xf0, 0x80, 0xf0, 0x80, 0x80, // F
};

unsigned char hardcoded_memory[2] = {0x30, 0x11};

// initialize all variables
void initialize(chip8_t *chip8) {
    chip8->PC = 0x200;	// all chip8 programs start at 0x200, so set PC to point in that address
    chip8->I = 0;
    chip8->SP = 0;
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
    chip8->opcode = 0;

    // load fontset on memory
    memcpy(&chip8->memory[0x000], sprites, sizeof(sprites));

    // *** hardcoded memory ***
    memcpy(&chip8->memory[0x200], hardcoded_memory, sizeof(hardcoded_memory));

    memset(&chip8->V, 0, sizeof(chip8->V));
    memset(&chip8->stack, 0, sizeof(chip8->stack));
    memset(&chip8->screen, 0, sizeof(chip8->screen));

    chip8->V[0] = 0x11;
}

// in a cycle the system should:
// 1.- fetch
// ------------------------------------------------------------------------------------
// During the fetch, the system will fetch one opcode from the memory at the location 
// specified by the PC. Data is stored in an array in which each address contains one byte. 
// As one opcode is 2 bytes long, we will need to fetch two seccessive bytes and merge them 
// to get the actual opcode
//
// memory[PC] = 0xA2
// memory[PC+1] = 0xF0
//
// In order to merge both bytes and store them in an unsigned short (2 bytes datatype) 
// we will use the bitwise OR operation:
//
// opcode = memory[PC] << 8 | memory[PC+1];
//
// So what did actually happen? First we shifted 0xA2 left 8 bits, which adds 8 zeros. 
//
// 0xA2		0xA2 << 8 = 0xA200	HEX
// 10100010	1010001000000000	BIN
//
// Next we use the bitwise OR operation to merge them: 
//
// 1010001000000000 | // 0xA200
//         11110000 = // 0xF0 (0x00F0)
// ------------------
// 1010001011110000   // 0xA2F0
// ------------------------------------------------------------------------------------
//
// 2.- decode
// ------------------------------------------------------------------------------------
// As we have stored our current opcode, we need to decode the opcode and check the opcode 
// table to see what it means. We will continue with the same opcode
//
// 0xA2F0 // assembly: mvi 2F0h
//
// If we take a look at the opcode table, it tells us the following:
//
// - ANNN: Sets I to the address NNN
//
// We will need to set index register I to the value of NNN (0x2F0) 
// ------------------------------------------------------------------------------------
//
// 3.- execute
// ------------------------------------------------------------------------------------
// Now that we know what to do with the opcode, we can execute the opcode in our emualator. 
// For our example instruction 0xA2F0 it means that we need to sotre the value 0x2F0 
// into index register I. As only 12 bits are containing the value we need to store, we 
// use bitwise AND operator (&) to get rid of the first four bits (nibble)
//
// 1010001011110000 & // 0xA2F0 (opcode)
// 0000111111111111 = // 0x0FFF
// ------------------
// 0000001011110000   // 0x02F0 (0x2F0) 
//
// Resulting code: 
//
// I = opcode & 0x0FFF; 
// pc += 2; 
//
// Because every instruction is 2 bytes long, we need to increment the program counter by 
// two after every executed opcode. This is true unless you jump to a certain address 
// in the memory or if you call a subroutine (in which case you need to store the program 
// counter in the stack). If the next opcode should be skipped, increase the program 
// counter by four.
// ------------------------------------------------------------------------------------
void emulateCycle(chip8_t *chip8) {
    chip8->opcode = chip8->memory[chip8->PC] << 8 | chip8->memory[chip8->PC+1];

    // *******************************************************************************
    // Because every instruction is 2 bytes long, we need to increment the program 
    // counter by two after every executed opcode. This is true unless you jump to a 
    // certain address in the memory or if you call a subroutine.
    // *******************************************************************************
    //
    // every opcode starts with a different and unique number, just compare the index
    switch (chip8->opcode & 0xF000) {
	case 0x0:
	    printf("opcode recibido: %hx\n", chip8->opcode);
	    if(chip8->opcode == 0x00E0) {
		printf("00E0 - CLS | Clear the display\n");
		chip8->PC += 2;
		break;
	    } else if (chip8->opcode == 0x00EE) {
		// the interpreter sets the PC to the address at the top of the stack
		// then subtracts 1 from the stack pointer
		printf("00EE - RET | Return from a subroutine\n");
		chip8->PC += 2;
		break;
	    }
	    break;
	case 0x1000:
	    printf("1nnn - JP addr | The interpreter sets the program counter to nnn\n");
	    printf("opcode recibido: %hx\n", chip8->opcode);
	    chip8->PC = chip8->opcode & 0x0FFF;
	    printf("CHIP8 PC: 0x%hx\n", chip8->PC);
	    break;
	case 0x2000:
	    // the interpreter increments the stack pointer, then puts the current PC on
	    // the top of the stack. The PC is then set to nnn
	    printf("2nnn - CALL addr | Call subroutine at nnn\n");
	    chip8->SP++;
	    chip8->stack[chip8->SP] = chip8->PC;
	    chip8->PC = chip8->opcode & 0x0FFF;
	    break;
	case 0x3000:
	    // the interpreter compares register Vx to kk, and if they are equal, increments 
	    // the PC by 2
	    printf("3xnn - SE Vx, byte | Skip next instruction if Vx = nn\n");
	    printf("%x\n", chip8->V[chip8->opcode & 0x0F00]);
	    printf("%x\n", chip8->opcode & 0x00FF);
	    if(chip8->V[chip8->opcode & 0x0F00] == (chip8->opcode & 0x00FF)) {
		chip8->PC += 2;
		printf("PC incremented\n");
	    }
	    chip8->PC += 2;
	    break;
	case 0x4000:
	    // the interpreter compares register Vx to nn, and if they are not equal, 
	    // increments the PC by 2
	    printf("4xnn - SNE Vx, byte | Skip next instruction if Vx = nn\n");
	    printf("%x\n", chip8->V[chip8->opcode & 0x0F00]);
	    printf("%x\n", chip8->opcode & 0x00FF);
	    if (chip8->V[chip8->opcode & 0xF00] != (chip8->opcode & 0x00FF)) {
		chip8->PC += 2;
		printf("PC incremented\n");
	    }
	    chip8->PC += 2;
	    break;
    }










    //printf("que valor hay en memoria en la direccion de PC: 0x%hhx\n", chip8->memory[chip8->PC]);
    //printf("que valor hay en memoria en la direccion de PC+1: 0x%02hhx\n", chip8->memory[chip8->PC+1]);
    //printf("que es PC: 0x%hx\n", chip8->PC);
    //printf("que es PC+1: 0x%hx\n", chip8->PC+1);
    //chip8->opcode = chip8->memory[chip8->PC];
    //printf("%hu\n", chip8->opcode);
}
