#include "isr.h"
#include "idt.h"
#include "../drivers/screen.h"
#include "../kernel/low_level.h"
#include "../common/system.h"

isr_t interrupt_handlers[256];

void init_irq_handle();

void register_interrupt_handle(unsigned char n, isr_t handle){
    interrupt_handlers[n] = handle;
}

void init_irq(){
    printk("Initialising interrupt...");
    init_irq_handle();
    asm volatile("sti");
    printk("Opened \n");
}

void init_irq_handle(){

}

// pass in parameters using stack.
void isr_handle(registers_t regs){

    if(interrupt_handlers[regs.int_no] != 0){
        isr_t handle = interrupt_handlers[regs.int_no];
        handle(regs);
    }else{
        printk("Interrupt No: "); printk_hex(regs.int_no); printk("\n");
        PANIC("No Interrupt handler");
    }

}


void irq_handle(registers_t regs){
    // end of interrupt.
    if(regs.int_no >= 40){
        port_byte_out(PIC2_COMMAND, PIC_EOI);
    }

    port_byte_out(PIC1_COMMAND, PIC_EOI);

    if(interrupt_handlers[regs.int_no] != 0){
        isr_t handle = interrupt_handlers[regs.int_no];
        handle(regs);
    }

    //printk("Finished Interrupt num: ");printk_hex(regs.int_no);
    //printk(" With IRQ num: ");printk_hex(regs.err_code);printk("\n");
}