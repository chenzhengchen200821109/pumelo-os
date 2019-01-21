#include "mem.h"
#include "stdio.h"
#include "assert.h"
#include "string.h"

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

static void* alloc_physical_pages(struct pool* m_pool)
{
    int bit_idx = bitmap_scan(&m_pool->pool_bitmap, 1);
    if (bit_idx == -1)
        return NULL;
    bitmap_set(&m_pool->pool_bitmap, bit_idx, 1);
    uint32_t page_paddr = (bit_idx * PAGE_SIZE) + m_pool->paddr_start;
    return (void *)page_paddr;
}

static void* get_virtual_pages(enum pool_flags pf, uint32_t pg_cnt)
{
    int vaddr_start = 0;
    int bit_idx_start = -1;
    uint32_t cnt = 0;

    if (pf == PF_KERNEL) {
        bit_idx_start = bitmap_scan(&kernel_vaddr.vaddr_bitmap, pg_cnt);
        if (bit_idx_start == - 1)
            return NULL;
        while (cnt < pg_cnt) {
            bitmap_set(&kernel_vaddr.vaddr_bitmap, bit_idx_start + cnt, 1);
            cnt++;
        }
        vaddr_start = kernel_vaddr.vaddr_start + bit_idx_start * PAGE_SIZE;
    } else {
        // for user space
    }
    return (void *)vaddr_start;
}

static uint32_t* pte_addr(uint32_t vaddr)
{
    uint32_t* pte = (uint32_t *)(0xffc00000 + ((vaddr & 0xffc00000) >> 10) + PTE_IDX(vaddr) * 4);
    return pte;
}

static uint32_t* pde_addr(uint32_t vaddr)
{
    uint32_t* pde = (uint32_t *)((0xfffff000) + PDE_IDX(vaddr) * 4);
    return pde;
}

static void insert_page_table(void* vaddr, void* paddr)
{
    uint32_t _vaddr = (uint32_t)vaddr;
    uint32_t _paddr = (uint32_t)paddr;
    uint32_t* pde = pde_addr(_vaddr);
    uint32_t* pte = pte_addr(_vaddr);

    if (*pde & 0x1) {
        if (!(*pte & 0x1)) {
            *pte = (_paddr | PG_US_U | PG_RW_W | PG_P_1);
        } else {
            panic("pte existed");
        }
    } else {
        uint32_t pde_paddr = (uint32_t)alloc_physical_pages(&kernel_pool);
        *pde = (pde_paddr | PG_US_U | PG_RW_W | PG_P_1);
        memset((void *)((int)pte & 0xfffff000), 0, PAGE_SIZE);
        *pte = (_paddr | PG_US_U | PG_RW_W | PG_P_1);
    }
}

 void* malloc_pages(enum pool_flags pf, uint32_t pg_cnt)
{
    void* vaddr_start = get_virtual_pages(pf, pg_cnt);
    if (!vaddr_start)
        return NULL;

    uint32_t vaddr = (uint32_t)vaddr_start, cnt = pg_cnt;
    struct pool* mem_pool = pf & PF_KERNEL ? &kernel_pool : &user_pool;

    while (cnt-- > 0) {
        void* page_paddr = alloc_physical_pages(mem_pool);
        if (page_paddr == NULL)
            return NULL;
        insert_page_table((void *)vaddr, page_paddr);
        vaddr += PAGE_SIZE;
    }
    return vaddr_start;
}

void* get_kernel_pages(uint32_t pg_cnt)
{
    void* vaddr = malloc_pages(PF_KERNEL, pg_cnt);
    if (vaddr != NULL)
        memset(vaddr, 0, pg_cnt * PAGE_SIZE);
    return vaddr;
}

void mem_init()
{
    kprintf("mem_init start\n");
    uint32_t mem_bytes_total = (*(uint32_t *)(0xd00));
    mem_pool_init(mem_bytes_total);
    kprintf("mem_init done\n");
}

