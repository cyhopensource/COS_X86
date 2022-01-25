// loaded 31 sector from the 2ed sectors of floppy.
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "../cpu/gdt.h"
#include "../cpu/timer.h"
#include "paging.h"

void test_addr(char* addr){
    *(addr + 10) = 'Y';
}

void init_drivers(){

    printk("Initialising system drivers...");
    init_keyboard();

    // mouse

    // audio

    // USB

    printk("ok\n");
}

// init os base system
void init_sys(){

    // interrupt
    init_gdt();
    init_idt();  // initialise interrupt table
    init_irq();  // initialise interrupt
    init_timer(200); // initialise intern timer
    
    // drivers interface
    init_drivers();

    //clear_screen();
    // memory
    init_paging();  // initialise page table

    // filesystem VFS

    // user shell interface (multi process stage)

}

// kernel entry.
void main(){

    clear_screen();
    printk("Welcome to COS -- version:"); printk_hex(0x01); printk(" 23/01/2022 \n");
    printk("caoyiheng43@gmail.com\n");
    printk("GPL 2022 Copyright\n");
    printk("Starting Kernel...\n");
    printk("......\n\n");

    printk_hex(0xaa55);
    printk("\n");

    // init os base system
    init_sys();

    printk("Switching into User's Mode\n");
    printk("......\n\n");

    unsigned int *ptr = (unsigned int*)0x00300000; // just 1024 pages(4KB) for now
    unsigned int do_page_fault = *ptr;

    // init GUI system GNOME_based

    //__asm__ ("int $0x3");
    //__asm__ ("int $0x5");
    //__asm__ ("int $0x22");
}

