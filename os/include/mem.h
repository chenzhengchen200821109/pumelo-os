#ifndef _MEM_H
#define _MEM_H

#include "defs.h"
#include "bitmap.h"

enum pool_flags
{
    PF_KERNEL = 1,
    PF_USER = 2
};

#define PG_P_1  1
#define PG_P_0  0
#define PG_RW_R 0
#define PG_RW_W 2
#define PG_US_S 0
#define PG_US_U 4

#define PDE_IDX(addr) ((addr & 0xffc00000) >> 22)
#define PTE_IDX(addr) ((addr & 0x003ff000) >> 12) 

struct virtual_addr
{
    struct bitmap vaddr_bitmap;
    uint32_t vaddr_start;
};

extern struct pool kernel_pool, user_pool;

void mem_init();
void* kmalloc_pages(uint32_t pg_cnt);

#endif
