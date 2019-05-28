#include "mem.h"
#include "stdio.h"
#include "assert.h"
#include "string.h"
#include "thread.h"
#include "sync.h"
#include "intr.h"
#include "x86.h"

#define PAGE_SIZE 4096

#define MEM_BITMAP_BASE 0xc009a000
#define KERNEL_HEAP_START 0xc0100000

struct pool
{
    struct bitmap pool_bitmap;
    uint32_t paddr_start;
    uint32_t pool_size;
	struct lock lk;
};

struct pool kernel_pool, user_pool;
struct virtual_addr kernel_vaddr;
struct mem_block_desc k_block_descs[DESC_CNT];

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
	
	lock_init(&kernel_pool.lk);
    lock_init(&user_pool.lk);
    kprintf("mem_pool_init done\n");

}

// allocate physical memory in page
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
        struct thread_struct *cur = running_thread();
        bit_idx_start = bitmap_scan(&cur->userprog_vaddr.vaddr_bitmap, pg_cnt);
        if (bit_idx_start == -1)
            return NULL;
        while (cnt < pg_cnt) {
            bitmap_set(&cur->userprog_vaddr.vaddr_bitmap, bit_idx_start + cnt, 1);
            cnt++;
        }
        vaddr_start = cur->userprog_vaddr.vaddr_start + bit_idx_start * PAGE_SIZE;
        assert((uint32_t)vaddr_start < (0xc0000000 - PAGE_SIZE));
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
	//kprintf_lock("*pte = 0x%x\n", *pte);

    if (*pde & 0x00000001) {
        if (!(*pte & 0x00000001)) {
            *pte = (_paddr | PG_US_U | PG_RW_W | PG_P_1);
        } else {
            //panic("pte existed"); // how this happen?
			*pte = (_paddr | PG_US_U | PG_RW_W | PG_P_1);
        }
    } else {
        uint32_t pde_paddr = (uint32_t)alloc_physical_pages(&kernel_pool);
        *pde = (pde_paddr | PG_US_U | PG_RW_W | PG_P_1);
        memset((void *)((int)pte & 0xfffff000), 0, PAGE_SIZE);
        *pte = (_paddr | PG_US_U | PG_RW_W | PG_P_1);
    }
}

static struct mem_block* arena_to_block(struct arena* a, uint32_t idx)
{
	return (struct mem_block *)((uint32_t)a + sizeof(struct arena) + idx * a->desc->block_size);
}

static struct arena* block_to_arena(struct mem_block* b)
{
	return (struct arena *)((uint32_t)b & 0xfffff000);
}

static void remove_page_table(uint32_t vaddr)
{
	uint32_t* pte = pte_addr(vaddr);
	*pte &= ~PG_P_1;
	asm volatile ("invlpg %0" : : "m" (vaddr) : "memory");
}

static void remove_virtual_pages(enum pool_flags pf, void* _vaddr, uint32_t pg_cnt)
{
	uint32_t bit_idx_start = 0;
	uint32_t vaddr = (uint32_t)_vaddr;
	uint32_t cnt = 0;

	if (pf == PF_KERNEL) {
		bit_idx_start = (vaddr - kernel_vaddr.vaddr_start) / PAGE_SIZE;
		while (cnt < pg_cnt) {
			bitmap_set(&kernel_vaddr.vaddr_bitmap, bit_idx_start + cnt, 0);
			cnt++;
		}
	} else {  // user space
		struct thread_struct* cur_thread = running_thread();
		bit_idx_start = (vaddr - cur_thread->userprog_vaddr.vaddr_start) / PAGE_SIZE;
		while (cnt < pg_cnt) {
			bitmap_set(&cur_thread->userprog_vaddr.vaddr_bitmap, bit_idx_start + cnt, 0);
			cnt++;
		}
	}
}

/* virtual address converted into physical address */
uint32_t addr_v2p(uint32_t vaddr)
{
	uint32_t* pte = pte_addr(vaddr);
	return ((*pte & 0xfffff000) + (vaddr & 0x00000fff));
}

// release physical memory in page
void free_physical_pages(uint32_t pg_phy_addr)
{
	struct pool* mem_pool;
	uint32_t bit_idx = 0;
	if (pg_phy_addr >= user_pool.paddr_start) {
		mem_pool = &user_pool;
		bit_idx = (pg_phy_addr - user_pool.paddr_start) / PAGE_SIZE;
	} else {
		mem_pool = &kernel_pool;
		bit_idx = (pg_phy_addr - kernel_pool.paddr_start) / PAGE_SIZE;
	}
	bitmap_set(&mem_pool->pool_bitmap, bit_idx, 0);
}

void* sys_malloc(uint32_t size)
{
	enum pool_flags PF;
	struct pool* mem_pool;
	uint32_t pool_size;
	struct mem_block_desc* desc;
	struct thread_struct* cur_thread = running_thread();

	if (cur_thread->pgdir == NULL) { // kernel thread
		PF = PF_KERNEL;
		pool_size = kernel_pool.pool_size;
		mem_pool = &kernel_pool;
		desc = k_block_descs;
	} else {
		PF = PF_USER;
		pool_size = user_pool.pool_size;
		mem_pool = &user_pool;
		desc = cur_thread->u_block_desc;
	}

	if (!(size > 0 && size < pool_size))
		return NULL;
	
	struct arena* a;
	struct mem_block* b;
	lock_acquire(&mem_pool->lk);
	
	if (size > 1024) {
		uint32_t page_cnt = DIV_ROUND_UP(size + sizeof (struct arena), PAGE_SIZE);
		
		a = (struct arena *)malloc_pages(PF, page_cnt);
		if (a != NULL) {
			memset(a, 0, page_cnt * PAGE_SIZE);
			a->desc = NULL;
			a->cnt = page_cnt;
			a->large = true;
			lock_release(&mem_pool->lk);
			return (void *)(a + 1);
		} else {
			lock_release(&mem_pool->lk);
			return NULL;
		}
	} else {
		uint8_t desc_idx;
		for (desc_idx = 0; desc_idx < DESC_CNT; desc_idx++) {
			if (size < k_block_descs[desc_idx].block_size)
				break;
		}
		
		if (list_empty(&k_block_descs[desc_idx].free_list)) {
			a = (struct arena *)malloc_pages(PF, 1);
			if (a == NULL) {
				lock_release(&mem_pool->lk);
				return NULL;
			}
			memset(a, 0, PAGE_SIZE);
			a->desc = &k_block_descs[desc_idx];
			a->large = false;
			a->cnt = k_block_descs[desc_idx].block_per_arena;
			uint32_t block_idx;
			
			enum intr_status old_status = intr_disable();
			
			for (block_idx = 0; block_idx < k_block_descs[desc_idx].block_per_arena; block_idx++) {
				b = arena_to_block(a, block_idx);
				assert(!list_find(&a->desc->free_list, &b->free_elem));
				list_append(&a->desc->free_list, &b->free_elem);
			}
			set_intr_status(old_status);
		}

		b = to_struct(list_pop(&(k_block_descs[desc_idx].free_list)), struct mem_block, free_elem);
		memset(b, 0, k_block_descs[desc_idx].block_size);
		
		a = block_to_arena(b);
		a->cnt--;
		lock_release(&mem_pool->lk);
		return (void *)b;
	}
}

void sys_free(void* ptr)
{
	assert(ptr != NULL);
	if (ptr != NULL) {
		enum pool_flags PF;
		struct pool* mem_pool;

		if (running_thread()->pgdir == NULL) {
			assert((uint32_t)ptr >= KERNEL_HEAP_START);
			PF = PF_KERNEL;
			mem_pool = &kernel_pool;
		} else {
			PF = PF_USER;
			mem_pool = &user_pool;
		}

		lock_acquire(&mem_pool->lk);
		struct mem_block* b = ptr;
		struct arena* a = block_to_arena(b);

		assert(a->large == 0 || a->large == 1);
		if (a->desc == NULL && a->large == true) {
			remove_virtual_pages(PF, a, a->cnt);
		} else {
			list_append(&a->desc->free_list, &b->free_elem);
			if (++a->cnt == a->desc->block_per_arena) {
				uint32_t block_idx;
				for (block_idx = 0; block_idx < a->desc->block_per_arena; block_idx++) {
					struct mem_block* b = arena_to_block(a, block_idx);
					assert(list_find(&a->desc->free_list, &b->free_elem));
					list_remove(&b->free_elem);
				}
				remove_virtual_pages(PF, a, 1);
			}
		}
		lock_release(&mem_pool->lk);
	}
}

void block_desc_init(struct mem_block_desc* desc_array)
{
	uint16_t desc_idx, block_size = 16;

	for (desc_idx = 0; desc_idx < DESC_CNT; desc_idx++) {
		desc_array[desc_idx].block_size = block_size;
		desc_array[desc_idx].block_per_arena = (PAGE_SIZE - sizeof(struct arena)) / block_size;
		list_init(&desc_array[desc_idx].free_list);
		block_size *= 2;
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

void free_pages(enum pool_flags pf, void* _vaddr, uint32_t pg_cnt)
{
	uint32_t pg_phy_addr;
	uint32_t vaddr = (int32_t)_vaddr;
	uint32_t page_cnt = 0;
	
	assert(pg_cnt >= 1 && vaddr % PAGE_SIZE == 0);
	pg_phy_addr = addr_v2p(vaddr);

	assert((pg_phy_addr % PAGE_SIZE) == 0 && pg_phy_addr >= 0x102000);

	if (pg_phy_addr >= user_pool.paddr_start) {
		vaddr -= PAGE_SIZE;
		while (page_cnt < pg_cnt) {
			vaddr += PAGE_SIZE;
			pg_phy_addr = addr_v2p(vaddr);
			assert((pg_phy_addr % PAGE_SIZE) == 0 && pg_phy_addr >= user_pool.paddr_start);
			free_physical_pages(pg_phy_addr);
			remove_page_table(vaddr);
			page_cnt++;
		}
		remove_virtual_pages(pf, _vaddr, pg_cnt);
	} else {
		vaddr -= PAGE_SIZE;
		while (page_cnt < pg_cnt) {
			vaddr += PAGE_SIZE;
			pg_phy_addr = addr_v2p(vaddr);
			assert((pg_phy_addr % PAGE_SIZE) == 0 && pg_phy_addr >= kernel_pool.paddr_start && pg_phy_addr < user_pool.paddr_start);
			free_physical_pages(pg_phy_addr);
			remove_page_table(vaddr);
			page_cnt++;
		}
		remove_virtual_pages(pf, _vaddr, pg_cnt);
	}
}

void* get_kernel_pages(uint32_t pg_cnt)
{
    void* vaddr = malloc_pages(PF_KERNEL, pg_cnt);
    if (vaddr != NULL)
        memset(vaddr, 0, pg_cnt * PAGE_SIZE);
    return vaddr;
}

void* get_user_pages(uint32_t pg_cnt)
{
    lock_acquire(&user_pool.lk);
    void* vaddr = malloc_pages(PF_USER, pg_cnt);
    memset(vaddr, 0, pg_cnt * PAGE_SIZE);
    lock_release(&user_pool.lk);
    return vaddr;
}

void* get_a_page(enum pool_flags pf, uint32_t vaddr)
{
    struct pool *mem_pool= pf & PF_KERNEL ? &kernel_pool : &user_pool;
    lock_acquire(&mem_pool->lk);

    struct thread_struct *cur = running_thread();
    int32_t bit_idx = -1;

    if (cur->pgdir != NULL && pf == PF_USER) {
        bit_idx = (vaddr - cur->userprog_vaddr.vaddr_start) / PAGE_SIZE;
        assert(bit_idx > 0);
        bitmap_set(&cur->userprog_vaddr.vaddr_bitmap, bit_idx, 1);
    } else if (cur->pgdir == NULL & pf == PF_KERNEL) {
        bit_idx = (vaddr - kernel_vaddr.vaddr_start) / PAGE_SIZE;
        assert(bit_idx > 0);
        bitmap_set(&kernel_vaddr.vaddr_bitmap, bit_idx, 1);
    } else {
        panic("get_a_page: not allow kernel alloc userspace or user alloc kernelspac by get_a_page");
    }
    void* page_phyaddr = alloc_physical_pages(mem_pool);
    if (page_phyaddr == NULL)
        return NULL;
    insert_page_table((void *)vaddr, page_phyaddr);
    lock_release(&mem_pool->lk);
    return (void *)vaddr;
}

void mem_init()
{
    kprintf("mem_init start\n");
    uint32_t mem_bytes_total = (*(uint32_t *)(0xd00));
    mem_pool_init(mem_bytes_total);
	block_desc_init(k_block_descs);
	lock_init(&kernel_pool.lk);
	lock_init(&user_pool.lk);
    kprintf("mem_init done\n");
}

