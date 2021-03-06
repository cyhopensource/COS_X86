#ifndef _ORDERDE_ARRAY_H_
#define _ORDERDE_ARRAY_H_

#include "type.h"

typedef void* type_t;

typedef u8int (*lessthan_predicate_t)(type_t, type_t);

typedef struct{
    type_t *array;  // void**
    u32int size;
    u32int max_size;
    lessthan_predicate_t less_than;
} ordered_array_t;

u8int standard_lessthan_predicate(type_t a, type_t b);

ordered_array_t create_ordered_array(u32int max_size, lessthan_predicate_t less_than);
ordered_array_t place_ordered_array(void* addr, u32int max_size, lessthan_predicate_t less_than);

void destroy_ordered_array(ordered_array_t *array);

u8int insert_ordered_array(type_t item, ordered_array_t *array);

type_t lookup_ordered_array(u32int i, ordered_array_t* array);

u8int remove_ordered_array(u32int i, ordered_array_t* array);

#endif


