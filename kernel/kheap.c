
#include "kheap.h"
#include "../kernel/kmem.h"
#include "../kernel/paging.h"

extern page_directory_t* kernel_directory;

// turing complet

u8int header_t_less_than(void* a, void* b){
    return (((header_t*)a)->size < ((header_t*)b)->size) ? 1 : 0;
}

// kernel_heap and user_mode_heap

u32int find_smallest_hole(u32int size, u8int page_ligne, heap_t *heap){
    u32int iterator = 0;
    while(iterator < heap -> index.size){
        header_t *header = (header_t*)lookup_ordered_array(iterator, &heap -> index);
        if(page_ligne  > 0){
            u32int location = (u32int)header;
            u32int offset = 0;
            if((location + sizeof(header_t)) & 0xFFFFF000 != 0)
                offset = 0x1000 - (location + sizeof(header_t)) % 0x1000;
            u32int hole_size = (u32int)header -> size - offset;

            if(hole_size >= (u32int)size)
                break;
        }
        else if(header -> size >= size)
            break;
        iterator++;
    }

    if(iterator == heap -> index.size)
        return -1;
    else
        return iterator;
}

// start addr and end addr should be aligned
heap_t *create_heap(u32int start, u32int end_addr, u32int max, u8int super, u8int readonly){
    heap_t *heap = (heap_t*)kmalloc(sizeof(heap_t));

    heap -> index = place_ordered_array((void*)start, HEAP_INDEX_SIZE, &header_t_less_than);

    start += sizeof(type_t)*HEAP_INDEX_SIZE;

    if(start & 0xFFFFF000 != 0){
        start &= 0xFFFFF000;
        start += 0x1000;   // 4kb page
    }

    heap -> start_addr = start;
    heap -> end_addr = end_addr;
    heap -> max_addr = max;
    heap -> super = super;
    heap -> readonly = readonly;

    // start with a large hole
    header_t* hole = (header_t*)start;
    hole -> size = end_addr - start;
    hole -> magic = HEAP_MAGIC;
    hole -> is_hole = 1;
    insert_ordered_array((void*)hole, &heap->index);

    return heap;
}

void expand(u32int new_size, heap_t* heap){
    if(new_size & 0xFFFFF000 != 0){
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }

    u32int old_size = heap -> end_addr - heap -> start_addr;
    u32int i = old_size;
    while(i < new_size){
        alloc_frame(get_page(heap->start_addr + i, 1, kernel_directory), (heap -> super) ? 1: 0, 
            (heap -> readonly) ? 1: 0);
        i += 0x1000;
    }
    heap -> end_addr = heap -> start_addr + i;  // here to prevent to zoom out
}

u32int contract(u32int new_size, heap_t* heap){
    if(new_size & 0xFFFFF000){
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }

    if(new_size < HEAP_MIN_SIZE)
        new_size = HEAP_MIN_SIZE;
    u32int old_size = heap -> end_addr - heap -> start_addr;
    u32int i = old_size - 0x1000;
    while(new_size < i){
        free_frame(get_page(heap -> start_addr + i, 0, kernel_directory));
        i -= 0x1000;
    }
    heap -> end_addr = heap -> start_addr + i;
    return new_size;
}

void* alloc(u32int size, u8int page_align, heap_t* heap){
    u32int new_size = size + sizeof(footer_t) + sizeof(header_t);
    u32int iterator = find_smallest_hole(new_size, page_align, heap);

    if(iterator == -1){

    }
    header_t *orig_hole_header = (header_t*)lookup_ordered_array(iterator, &heap -> index);
    u32int orig_hole_pos = (u32int)orig_hole_header;
    u32int orig_hole_size = orig_hole_header -> size;

    if(orig_hole_size - new_size < sizeof(header_t) + sizeof(footer_t)){
        size += orig_hole_size - new_size;
        new_size = orig_hole_size;
    }    
}








