#include "string.h"

int strlen(char* str){
    int len = 0;
    while(str[len] != '\0')
        len += 1;
    return len;
}

void append_char(char* str, char app){
    int len = strlen(str);
    str[len] = app;
    str[len + 1] = '\0';
}
