#include "timer.h"
#include "isr.h"
#include "../kernel/low_level.h"
#include "../drivers/screen.h"

unsigned int tick = 0;
unsigned int second = 0;

void timer_callback(registers_t regs){
    tick++;
    if(tick == 200){   // for 1 second.
        //printk("Tick: ");
        //printk_hex(second++);
        //printk("s\n");
        tick = 0;
    }
}

void init_timer(unsigned int freq){
    register_interrupt_handle(IRQ0 , timer_callback);  // interrupt id.

    unsigned int divisor = 1193180 / freq;

    port_byte_out(TIMER_COMMAND, 0x36);

    unsigned char l = (unsigned char)(divisor & 0xff);
    unsigned char h = (unsigned char)((divisor >> 8) & 0xff);

    port_byte_out(TIMER_DATA, l);
    port_byte_out(TIMER_DATA, h);
}