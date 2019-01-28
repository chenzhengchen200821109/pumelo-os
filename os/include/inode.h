#ifndef _INODE_H
#define _INODE_H

#include "defs.h"
#include "list.h"

struct inode 
{
    uint32_t i_no;
    uint32_t i_size;

    uint32_t i_open_cnts;
    bool write_only;

    uint32_t i_sectos[13];
    struct list_entry inode_tag;
};

#endif
