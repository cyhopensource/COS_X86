#include "paging.h"
#include "kmem.h"
#include "util.h"
#include "../common/type.h"
#include "../common/system.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"

u32int *frames;
u32int nframes;

page_directory_t* kernel_directory;
page_directory_t* current_directory;

extern u32int free_addr;

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

void page_fault(registers_t regs);

void set_frame(u32int frame_addr){
    u32int frame = frame_addr / 0x1000;
    u32int idx = INDEX_FROM_BIT(frame);
    u32int off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

void clear_frame(u32int frame_addr){
    u32int frame = frame_addr / 0x1000;
    u32int idx = INDEX_FROM_BIT(frame);
    u32int off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

u32int test_frame(u32int frame_addr){
    u32int frame = frame_addr / 0x1000;
    u32int idx = INDEX_FROM_BIT(frame);
    u32int off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

// return id of the first frame. 
u32int first_frame(){
    u32int i, j;
    for(i = 0; i < INDEX_FROM_BIT(nframes); i++){
        if(frames[i] != 0xFFFFFFFF){
            for(j = 0; j < 32; j++){
                u32int toTest = 0x1 << j;
                if(!(frames[i] & toTest)){
                    return i*4*8 + j;
                }
            }
        }
    }
    return (u32int)-1;
}

void alloc_frame(page_t *page, int is_kernel, int is_writable){
    if(page == 0)
        return;
    if(page -> frame != 0)
        return;
    else{
        //printk("alloc_frame\n");
        u32int idx = first_frame();  // allocate physical addr.
        
        if(idx == (u32int)-1){
            PANIC("No Free Frames");
        }

        set_frame(idx*0x1000); 
        page -> present = 1;
        page -> rw = (is_writable) ? 1 : 0;
        page -> user = (is_kernel) ? 0 : 1;
        page -> frame = idx; 
    }    
}

// free the physic frame to be availble
void free_frame(page_t* page){
    u32int frame;
    if(!(frame = page -> frame)){
        return;
    }else{
        clear_frame(frame);
        page -> frame = 0x0;  // free page.
    }
}

// virtual page.
page_t* get_page(u32int addr, int make, page_directory_t *dir){
    //printk("Getting page\n");

    addr /= 0x1000;
    u32int table_idx = addr / 1024;
    //u32int table_idx = addr / 16;
    // is already assigned.
    if(dir -> tables[table_idx]){
        //printk("Have had table\n");
        //printk("\n");
        return &dir->tables[table_idx]->pages[addr % 1024];
        //return &dir->tables[table_idx]->pages[addr % 16];
    }
    else if(make){
        u32int temp;
        dir -> tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &temp);
        memory_set(dir -> tables[table_idx], 0, 0x1000);  //*************************************************
        dir -> tablePhysical[table_idx] = temp | 0x7; // present RW US.

        //printk("maked\n");
        //printk_hex(&dir -> tables[table_idx] -> pages[addr % 1024]);
        //printk("\n");
        return &dir -> tables[table_idx] -> pages[addr % 1024];
        //return &dir -> tables[table_idx] -> pages[addr % 16];

    }
    else{
        return 0;
    }
}

void switch_page_directory(page_directory_t *dir){
    current_directory = dir;
    asm volatile("mov %0, %%cr3" :: "r"(&dir -> tablePhysical));
    //printk_hex(&dir -> tablePhysical);
    u32int cr0;
    asm volatile("mov %%cr0, %%eax" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %%eax, %%cr0" :: "r"(cr0));
    //asm volatile(
	//	"mov %0, %%cr3\n"
	//	"mov %%cr0, %%eax\n"
	//	"or $0x80000000, %%eax\n"
	//	"mov %%eax, %%cr0\n"
	//	:: "r"(dir->tablePhysical)
	//	: "%eax"
	//);
}

void init_paging(){
    
    // 16M
    //u32int mem_end_page = 0x1000000;
    u32int mem_end_page = 0x1000000;

    printk("Initialising paging...");
    // frame table page
    nframes = mem_end_page / 0x1000;   // 4KB page size.
    //printk_hex(nframes);
    //printk("\n");

    frames = (u32int*)kmalloc_a(INDEX_FROM_BIT(nframes));
    printk(" ");
    printk_hex(frames);
    printk(" ");
    memory_set(frames, 0, INDEX_FROM_BIT(nframes));
    
    
    // frame directory page
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    printk_hex(kernel_directory);
    printk(" ");
    memory_set(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;
    
    printk_hex(free_addr);
    printk(" ");
    int i = 0;
    while(i < 0xC00000){  // the number of page
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }

    
    register_interrupt_handle(14, page_fault);

    switch_page_directory(kernel_directory);

    printk("ok\n");
    
}

void page_fault(registers_t regs){
    u32int fault_addr;
    asm volatile("mov %%cr2, %0" : "=r"(fault_addr) : );
    int present = !(regs.err_code & 0x1);
    int rw = regs.err_code & 0x2;
    int us = regs.err_code & 0x4;
    int reserved = regs.err_code & 0x08;
    int id = regs.err_code & 0x10;

    printk("page fault! (");
    if(present) printk("present ");
    if(rw) printk("read-only ");
    if(us) printk("user-mode ");
    if(reserved) printk("reserved ");
    printk(") at");
    printk_hex(fault_addr);
    printk("\n");
    PANIC("Page Fault");
}

