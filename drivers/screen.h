#ifndef _SCREEN_H_
#define _SCREEN_H_

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

#define WHITE_ON_BLACK 0x0f

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

// function interface.
void printk(char* message);
void printk_hex(unsigned short num);
void clear_screen();

#endif
