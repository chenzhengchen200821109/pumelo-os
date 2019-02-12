#ifndef _MEM_H
#define _MEM_H

#include "defs.h"
#include "bitmap.h"
#include "list.h"

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

struct mem_block
{
	struct list_entry free_elem;
};

struct mem_block_desc
{
	uint32_t block_size;
	uint32_t block_per_arena;
	struct list free_list;
};

struct arena
{
	struct mem_block_desc* desc;
	uint32_t cnt;
	bool large;
};

#define DESC_CNT 7

extern struct pool kernel_pool, user_pool;

void mem_init();
void* malloc_pages(enum pool_flags pf, uint32_t pg_cnt);
void free_pages(enum pool_flags pf, void* _vaddr, uint32_t pg_cnt);
void* get_kernel_pages(uint32_t pg_cnt);
void* sys_malloc(uint32_t size);
void sys_free(void* ptr);

#endif
