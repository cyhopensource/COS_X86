#include "kmem.h"
#include "../common/type.h"
#include "../drivers/screen.h"

extern unsigned int __end;

unsigned int free_addr = (unsigned int)&__end;  
//unsigned int free_addr = 0x5000;  

unsigned int kmalloc_ap(unsigned int sz, unsigned int *phys){
    int align = 1;
    if(align == 1 && (free_addr & 0xFFFFF000)){
        free_addr &= 0xFFFFF000;
        free_addr += 0x1000;
    }
    if(phys){
        *phys = free_addr;
    }
    unsigned int temp = free_addr;
    free_addr += sz;
    return temp;
}

unsigned int kmalloc(unsigned int sz){
    unsigned int temp = free_addr;
    free_addr += sz;
    return temp;
}

unsigned int kmalloc_a(unsigned int sz){
    int align = 1;
    if(align == 1 && (free_addr & 0xFFFFF000)){
        free_addr &= 0xFFFFF000;
        free_addr += 0x1000;
    }
    
    unsigned int temp = free_addr;
    free_addr += sz;
    return temp;
}