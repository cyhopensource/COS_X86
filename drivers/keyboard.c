#include"keyboard.h"
#include "screen.h"
#include "../common/type.h"
#include "../cpu/isr.h"
#include "../kernel/low_level.h"
#include "../libc/string.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C

#define SC_MAX 57

const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

void keyboard_callback(){
    
    u8int scancode = port_byte_in(0x60);
    
    if(scancode > SC_MAX) return;
    if(scancode == BACKSPACE){
        printk(" ");
    }else if(scancode == ENTER){
        printk("\n");
    }else{
        char letter = sc_ascii[(int)scancode];
        char str[2] = {letter, '\0'};
        printk(str); 
    }   
}

void init_keyboard(){   
    register_interrupt_handle(IRQ1, (unsigned int)keyboard_callback);
}