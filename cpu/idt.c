#include "idt.h"
#include "../kernel/util.h"
#include "../kernel/low_level.h"
#include "../drivers/screen.h"

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

void idt_set_gate(unsigned char num, unsigned int base, unsigned short sel, unsigned char flags);
void idt_flush(unsigned int* _idt_ptr);

void init_idt(){
    printk("Initialising IDT...");

    idt_ptr.limit = sizeof(idt_entry_t)*256 - 1;
    idt_ptr.base = (unsigned int)&idt_entries;

    memory_set(&idt_entries, 0, sizeof(idt_entry_t)*256);

    // remap the irq table
    port_byte_out(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);
    port_byte_out(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
    port_byte_out(PIC1_DATA, PIC1_VECTOR_OFFSET);
    port_byte_out(PIC2_DATA, PIC2_VECTOR_OFFSET);
    port_byte_out(PIC1_DATA, 0x04);
    port_byte_out(PIC2_DATA, 0x02);
    port_byte_out(PIC1_DATA, ICW4_8086);
    port_byte_out(PIC2_DATA, ICW4_8086);
    port_byte_out(PIC1_DATA, 0x0);
    port_byte_out(PIC2_DATA, 0x0);

    idt_set_gate(0, (unsigned int)isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned int)isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned int)isr2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned int)isr3, 0x08, 0x8E);
    idt_set_gate(4, (unsigned int)isr4, 0x08, 0x8E);
    idt_set_gate(5, (unsigned int)isr5, 0x08, 0x8E);
    idt_set_gate(6, (unsigned int)isr6, 0x08, 0x8E);
    idt_set_gate(7, (unsigned int)isr7, 0x08, 0x8E);
    idt_set_gate(8, (unsigned int)isr8, 0x08, 0x8E);
    idt_set_gate(9, (unsigned int)isr9, 0x08, 0x8E);
    idt_set_gate(10, (unsigned int)isr10, 0x08, 0x8E);
    idt_set_gate(11, (unsigned int)isr11, 0x08, 0x8E);
    idt_set_gate(12, (unsigned int)isr12, 0x08, 0x8E);
    idt_set_gate(13, (unsigned int)isr13, 0x08, 0x8E);
    idt_set_gate(14, (unsigned int)isr14, 0x08, 0x8E);


    idt_set_gate(32, (unsigned int)irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned int)irq1, 0x08, 0x8E);
    idt_set_gate(34, (unsigned int)irq2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned int)irq3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned int)irq4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned int)irq5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned int)irq6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned int)irq7, 0x08, 0x8E);
    idt_set_gate(40, (unsigned int)irq8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned int)irq9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned int)irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned int)irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned int)irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned int)irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned int)irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned int)irq15, 0x08, 0x8E);

    idt_flush((unsigned int)&idt_ptr);
    
    printk("OK \n");
}

void idt_set_gate(unsigned char num, unsigned int base, unsigned short sel, unsigned char flags){
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
    idt_entries[num].sel = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags = flags;
}

void idt_flush(unsigned int* _idt_ptr){
    __asm__ __volatile__("lidtl (%0)" : : "r"(_idt_ptr));
}