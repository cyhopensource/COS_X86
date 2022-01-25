#ifndef SYSTEM_H
#define SYSTEM

#include "../drivers/screen.h"

#define PANIC(msg) printk("Panic: "); \
                printk(msg); \
                printk("\n"); \
                printk("OS Stop! \n");\
                while(1);



#endif