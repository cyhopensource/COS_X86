
#include "kheap.h"
#include "../common/ordered_array.h"
#include "../kernel/kmem.h"
#include "../kernel/paging.h"
#include "../drivers/screen.h"

extern page_directory_t* kernel_directory;
extern unsigned int __end;
unsigned int placement_address = (unsigned int)&__end;  
// turing complet
heap_t* kheap = 0;

u32int malloc_int(u32int sz, int align, u32int *phy_addr){
    if(kheap != 0){
        void* addr = alloc(sz, (u8int)align, kheap);
        if(phy_addr != 0){
            page_t* page = get_page((u32int)addr, 0, kernel_directory);
            *phy_addr = page -> frame*0x1000 + ((u32int)addr & 0xFFF);
        }
        return (u32int)addr;
    }else
        return 0;
}

u32int malloc(u32int sz){
    return malloc_int(sz, 0, 0);
}


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
    hole -> size = end_addr - start;  // big champs
    hole -> magic = HEAP_MAGIC;
    hole -> is_hole = 1;
    insert_ordered_array((void*)hole, &heap->index);
    //printk("Here is create: "); printk_hex(heap -> index.array[0]);printk("\n");

    return heap;
}

void expand(u32int new_size, heap_t* heap){
    if(new_size & 0xFFFFF000 != 0){
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }

    u32int old_size = heap -> end_addr - heap -> start_addr;
    u32int i = old_size;
    // allocate new page
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

        u32int old_length = heap -> end_addr - heap -> start_addr;
        u32int old_end_addr = heap -> end_addr;
        
        expand(old_length + new_size, heap);
        u32int new_length = heap -> end_addr - heap -> start_addr;

        iterator = 0;

        u32int idx = -1; u32int value = 0x0;

        while(iterator < heap -> index.size){
            u32int temp = (u32int)lookup_ordered_array(iterator, &heap->index);
            if(temp > value){
                value = temp;
                idx = iterator;
            }
            iterator++;
        }
        
        if(idx == -1){
            header_t* header = (header_t*)old_end_addr;
            header -> magic = HEAP_MAGIC;
            header -> size = new_length - old_length;
            header -> is_hole = 1;
            footer_t* footer = (footer_t*)(old_end_addr + header->size - sizeof(footer_t));
            footer -> magic = HEAP_MAGIC;
            footer -> header = header;
            insert_ordered_array((void*)header, &heap->index);
        }else{
            header_t* header = lookup_ordered_array(idx, &heap -> index);
            header -> size += new_length - old_length;
            footer_t *footer = (footer_t *) ( (u32int)header + header->size - sizeof(footer_t) );
            footer->header = header;
            footer->magic = HEAP_MAGIC;
        }
        return alloc(size, page_align, heap);

    }

    header_t *orig_hole_header = (header_t*)lookup_ordered_array(iterator, &heap -> index);
    printk_hex(heap -> index.array[0]); printk("\n");
    u32int orig_hole_pos = (u32int)orig_hole_header;
    u32int orig_hole_size = orig_hole_header -> size;

    if(orig_hole_size - new_size < sizeof(header_t) + sizeof(footer_t)){
        size += orig_hole_size - new_size;
        new_size = orig_hole_size;
    }

    if(page_align && orig_hole_pos & 0xFFFFF000){
        u32int new_location = orig_hole_pos + 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);
        header_t* hole_header = (header_t*)orig_hole_pos;
        hole_header -> size = 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);
        hole_header -> magic = HEAP_MAGIC;
        hole_header -> is_hole = 1;
        footer_t* hole_footer = (footer_t*)((u32int)new_location - sizeof(footer_t));
        hole_footer -> magic = HEAP_MAGIC;
        hole_footer -> header = hole_header;
        orig_hole_pos = new_location;
        orig_hole_size = orig_hole_size - hole_header -> size;
    }else{
        remove_ordered_array(iterator, &heap -> index);
    }

    header_t* block_header = (header_t*)orig_hole_pos;
    block_header -> magic = HEAP_MAGIC;
    block_header -> is_hole = 0;
    block_header -> size = new_size;

    footer_t* block_footer = (footer_t*)(orig_hole_pos + size + sizeof(header_t));
    block_footer -> magic = HEAP_MAGIC;
    block_footer -> header = block_header;

    if(orig_hole_size - new_size > 0){
        header_t* hole_header = (header_t*)(orig_hole_pos + sizeof(header_t) + size + sizeof(footer_t));
        hole_header -> magic = HEAP_MAGIC;
        hole_header -> is_hole = 1;
        hole_header -> size = orig_hole_size - new_size;
        footer_t* hole_footer = (footer_t*)((u32int)hole_header + orig_hole_size - new_size - sizeof(footer_t));
        if((u32int)hole_footer < heap -> end_addr){
            hole_footer -> magic = HEAP_MAGIC;
            hole_footer -> header = hole_header;
        }
        insert_ordered_array((void*)hole_footer, &heap->index);
    }

    return (void*)((u32int)block_header + sizeof(header_t));

}

void free(void* p, heap_t *heap){
    if(p == 0)
        return;

    header_t* header = (header_t*)((u32int)p - sizeof(header_t));
    footer_t* footer = (footer_t*)((u32int)header + header->size - sizeof(footer_t));

    if(header -> magic != HEAP_MAGIC)
        return;
    if(footer -> magic != HEAP_MAGIC)
        return;

    header -> is_hole = 1;

    char do_add = 1;

    footer_t* test_footer = (footer_t*)((header) - sizeof(footer));
    if(test_footer -> magic == HEAP_MAGIC && test_footer -> header ->is_hole == 1){
        u32int cache_size = header -> size;
        header = test_footer -> header;
        footer -> header = header;
        header -> size += cache_size; // unify.
        do_add = 0;
    }

    header_t* test_header = (header_t*)((u32int)footer + sizeof(footer_t));
    if(test_header -> magic == HEAP_MAGIC && test_header -> is_hole == 1){
        header -> size += test_header -> size;
        test_footer = ((u32int)test_header + test_header -> size - sizeof(footer_t));
        footer = test_footer;
        
        u32int iterator = 0;
        while((iterator < heap -> index.size) && 
            (lookup_ordered_array(iterator, &heap -> index) != (void*)test_header))
            iterator ++;

        if(iterator >= heap -> index.size)
            return;

        remove_ordered_array(iterator, &heap -> index);
    }

    if((u32int)footer + sizeof(footer_t) == heap -> end_addr){
        u32int old_length = heap -> end_addr - heap -> start_addr;
        u32int new_length = contract((u32int)header - heap -> start_addr, heap);

        if(header -> size - (old_length - new_length) > 0){
            header -> size -= old_length - new_length;
            footer = (footer_t*)((u32int)header + header -> size - sizeof(footer_t));
            footer -> magic = HEAP_MAGIC;
            footer -> header = header;
        }
        else{
            u32int iterator = 0;
            while((iterator < heap -> index.size) && 
            (lookup_ordered_array(iterator, &heap -> index) != test_header))
                iterator++;

            if(iterator < heap -> index.size)
                remove_ordered_array(iterator, &heap -> index);
        }
    }

    if(do_add == 1)
        insert_ordered_array((void*)header, &heap -> index);

}














