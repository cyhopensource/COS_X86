#include "../kernel/low_level.h"
#include "../kernel/util.h"
#include "screen.h"

int get_cursor(){
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);

    return offset * 2;
}

void set_cursor(int offset){
    offset /= 2;

    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

int get_screen_offset(unsigned int col, unsigned int row){
    return (row * MAX_COLS + col) * 2;
}

int handle_scrolling(int offset){

}

void print_char(char character, int col, int row, char attribute_byte){
    unsigned char *video_mem = (unsigned char*)VIDEO_ADDRESS;
    
    if(!attribute_byte){
        attribute_byte = WHITE_ON_BLACK;
    }

    int offset;

    if(col >= 0 && row >= 0){
        offset = get_screen_offset(col, row);
    }else{
        offset = get_cursor();
    }

    if(character == '\n'){
        int row = offset / (2*MAX_COLS);
        offset = get_screen_offset(0, row + 1);
    }else{
        video_mem[offset] = character;
        video_mem[offset + 1] = attribute_byte;
        offset += 2;
    }
    //offset = handle_scrolling(offset);

    set_cursor(offset);
}

void print_at(char* message, int col, int row){
    if(col >= 0 && row >= 0){
        set_cursor(get_screen_offset(col, row));
    }

    int i = 0;
    while(message[i] != 0){
        print_char(message[i++], col ,row, WHITE_ON_BLACK);
    }
}

void clear_screen(){
    int row = 0;
    int col = 0;

    for(row = 0; row < MAX_ROWS; row++)
        for(col = 0; col < MAX_COLS; col++){
            print_char(' ', col ,row, WHITE_ON_BLACK);
        }

    set_cursor(get_screen_offset(0, 0));
}

void printk(char* message){
    print_at(message, -1, -1);
}

void printk_hex(unsigned int num){
    char* msg_hex = 0;
    unsigned char hex, compte = 0;
    msg_hex[compte++] = '0';
    msg_hex[compte++] = 'x';
    for(int i = 7; i >= 0; i--){
        hex = (num >> (4 * i)) & 0x0F;
        if(hex < 10)
            hex += 48;
        else
            hex += 55;
        msg_hex[compte++] = hex;
    }
    msg_hex[compte++] = '\0';
    print_at(msg_hex, -1, -1);
}
