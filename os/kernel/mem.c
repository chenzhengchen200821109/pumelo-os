#include "mem.h"
#include "stdio.h"

#define PAGE_SIZE 4096

#define MEM_BITMAP_BASE 0xc009a000
#define KERNEL_HEAP_START 0xc0100000

struct pool
{
    struct bitmap pool_bitmap;
    uint32_t paddr_start;
    uint32_t pool_size;
};

struct pool kernel_pool, user_pool;
struct virtual_addr kernel_vaddr;

static void mem_pool_init(uint32_t all_mem)
{
    kprintf("mem_pool_init start\n");
    uint32_t page_table_size = PAGE_SIZE * 256;

    // 0x100000 = 1MB
    uint32_t used_mem = page_table_size + 0x100000;
    uint32_t free_mem = all_mem - used_mem;
    uint16_t all_free_pages = free_mem / PAGE_SIZE;

    uint16_t kernel_free_pages = all_free_pages / 2;
    uint16_t user_free_pages = all_free_pages - kernel_free_pages;

    uint32_t kbm_length = kernel_free_pages / 8;
    uint32_t ubm_length = user_free_pages / 8;

    uint32_t kp_start = used_mem;
    uint32_t up_start = kp_start + kernel_free_pages * PAGE_SIZE;

    kernel_pool.paddr_start = kp_start;
    user_pool.paddr_start = up_start;

    kernel_pool.pool_size = kernel_free_pages * PAGE_SIZE;
    user_pool.pool_size = user_free_pages * PAGE_SIZE;

    kernel_pool.pool_bitmap.bitmap_len = kbm_length;
    user_pool.pool_bitmap.bitmap_len = ubm_length;

    kernel_pool.pool_bitmap.bitmap_bits = (void *)MEM_BITMAP_BASE;
    user_pool.pool_bitmap.bitmap_bits = (void *)(MEM_BITMAP_BASE + kbm_length);

    kprintf("kernel_pool_bitmap_start: %x\n", kernel_pool.pool_bitmap.bitmap_bits);
    kprintf("kernel_pool_paddr_start: %x\n", kernel_pool.paddr_start);

    bitmap_init(&kernel_pool.pool_bitmap);
    bitmap_init(&user_pool.pool_bitmap);

    kernel_vaddr.vaddr_bitmap.bitmap_len = kbm_length;
    kernel_vaddr.vaddr_bitmap.bitmap_bits = (void *)(MEM_BITMAP_BASE + kbm_length + ubm_length);

    kernel_vaddr.vaddr_start = KERNEL_HEAP_START;
    bitmap_init(&kernel_vaddr.vaddr_bitmap);
    kprintf("mem_pool_init done\n");

}

void* alloc_pages(size_t n)
{
    int bit_idx;
    if ((bit_idx = bitmap_scan(&(kernel_pool.pool_bitmap), n)) != -1)
        return (void *)(kernel_pool.paddr_start + bit_idx * PAGE_SIZE);
}

void free_pages(void* base, size_t n)
{
    int bit_idx;

    bit_idx = (int)(base - kernel_pool.paddr_start) / 4096;
    for (size_t i = 0; i < n; i++) {
        bitmap_set(&(kernel_pool.pool_bitmap), bit_idx, 0);
        bit_idx++;
    }
}

void mem_init()
{
    kprintf("mem_init start\n");
    uint32_t mem_bytes_total = (*(uint32_t *)(0xb00));
    mem_pool_init(mem_bytes_total);
    kprintf("mem_init done\n");
}

