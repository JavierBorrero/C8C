#include "chip8.h"

int main(void) {
    // setup render system and register input callbacks
    //setupGraphics();
    //setupInput();
    
    chip8_t chip8;

    // initialize chip8 and load the game
    initialize(&chip8);
    emulateCycle(&chip8);
    //loadGame(**argv);

    // emulation loop
    //for(;;) {
    //    // emulate one cycle
    //    emulateCycle();

    //    // if the draw flag is set, update the screen
    //    if(my_chip8.drawFlag)
    //        drawGraphics();

    //    // store key press state
    //    setKeys();
    //}

    return 0;
}
