#ifndef PAGING_H
#define PAGING_H

typedef struct page{
    unsigned int present  :1;
    unsigned int rw       :1;
    unsigned int user     :1;
    unsigned int accessed :1;
    unsigned int dirty    :1;
    unsigned int unused   :7;
    unsigned int frame    :20;
} page_t;

typedef struct page_table{
    page_t pages[1024];
    //page_t pages[16];
} page_table_t;

// 2-level
typedef struct page_directory{
    page_table_t* tables[1024];
    //page_table_t* tables[16];
    //unsigned int tablePhysical[1024];
    unsigned int tablePhysical[16];
    unsigned int physicalAddr;

} page_directory_t;

void init_paging();

void switch_page_directory(page_directory_t *new_page_dir);

page_t* get_page(unsigned int addr, int make, page_directory_t *dir);

void alloc_frame(page_t *page, int is_kernel, int is_writable);

#endif