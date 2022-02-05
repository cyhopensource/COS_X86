#ifndef _VFS_H_
#define _VFS_H_

#include "../common/type.h"

typedef struct fs_node
{
    char name[128];
    u32int flags;
    u32int uid;
    u32int gid;
    u32int mask;
    u32int inode;
    u32int length;
    u32int impl;
    // functions
    struct fs_node* ptr;
} fs_node_t;

#endif