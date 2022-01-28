
#include "kheap.h"

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





