typedef struct{
    unsigned int magic;
    unsigned char is_hole;
    unsigned int size;  // size of the bloc.
} header_t;

typedef struct{
    unsigned int magic;
    header_t *header; 
} footer_t;

typedef void* type_t;


