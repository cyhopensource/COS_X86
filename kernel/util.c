#include"util.h"

void memory_copy(char* source, char* dest, int no_bytes){
    int i;
    for(i = 0; i < no_bytes; i++){
        *(dest + i) = *(source + i);
    }
}

void memory_set(char* dest, char val, unsigned int len){
    char* temp = dest;
    for(; len != 0; len--)
        *temp++ = val;
}