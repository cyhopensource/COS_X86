#include "ordered_array.h"
#include "../kernel/kmem.h"
#include "../kernel/util.h"
#include "../drivers/screen.h"

// considering as the c compiler
u8int standard_lessthan_predicate(type_t a, type_t b){
    return (a < b) ? 1 : 0;
}

// max_size like c++ container
ordered_array_t create_ordered_array(u32int max_size, lessthan_predicate_t less_than){
    ordered_array_t ret;
    ret.array = (type_t*)kmalloc(max_size*sizeof(type_t));
    memory_set(ret.array, 0, max_size*sizeof(type_t));
    ret.size = 0;
    ret.max_size = max_size;
    ret.less_than = less_than;
    return ret;
}

ordered_array_t place_ordered_array(void* addr, u32int max_size, lessthan_predicate_t less_than){
    ordered_array_t ret;
    ret.array = (type_t*)addr;

    memory_set(ret.array, 0, max_size*sizeof(type_t));
    ret.size = 0;
    ret.max_size = max_size;
    ret.less_than = less_than;
    return ret;
}

void destroy_ordered_array(ordered_array_t *array){
    // kfree()
}

u8int insert_ordered_array(type_t item, ordered_array_t *array){
    u32int iterator = 0;
    if(array -> size == array -> max_size)
        return 0;

    while(iterator < array -> size && array -> less_than(array -> array[iterator], item))
        iterator++;

    if(iterator == array->size){
        array->array[array->size++] = item;
    }
    else{
        type_t temp = array -> array[iterator];
        array -> array[iterator] = item;
        while(iterator < array -> size){
            iterator++;
            type_t temp_2 = array -> array[iterator];
            array -> array[iterator] = temp;
            temp = temp_2;
        }
        array -> size++;
    }
    return 1;

}

type_t lookup_ordered_array(u32int i, ordered_array_t *array){
    return array->array[i];
}

// i < size
u8int remove_ordered_array(u32int i, ordered_array_t *array){
    if(i >= array -> size)
        return 0;
    while(i < array -> size){
        array -> array[i] = array -> array[i + 1];
        i++;
    }
    array -> size --;
    return 1;
}




