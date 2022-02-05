#ifndef _KHEAP_H_
#define _KHEAP_H_

#include "../common/ordered_array.h"

#define KHEAP_START      0xC0000000
//#define KHEAP_INIT_SIZE  0x100000
#define KHEAP_INIT_SIZE  0x100000
#define HEAP_INDEX_SIZE  0x20000
#define HEAP_MAGIC       0x123AB0BC   // magic number
#define HEAP_MIN_SIZE    0x70000 

typedef struct{
    unsigned int magic;
    unsigned char is_hole;
    unsigned int size;  // size of the bloc.
} header_t;

typedef struct{
    unsigned int magic;
    header_t *header; 
} footer_t;

typedef struct{
    ordered_array_t index;   // low addr in struct
    u32int start_addr;
    u32int end_addr;
    u32int max_addr;
    u8int  super;
    u8int readonly;    // high addr in struct
} heap_t;

heap_t *create_heap(u32int start, u32int end, u32int max, u8int super, u8int readonly);

void *alloc(u32int size, u8int page_align, heap_t *heap);

void free(void *p, heap_t *heap);

u32int malloc_int(u32int sz, int align, u32int *phy_addr);
u32int malloc(u32int sz);

#endif


