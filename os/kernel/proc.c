#include "thread.h"
#include "tss.h"
#include "defs.h"
#include "bitmap.h"
#include "list.h"
#include "proc.h"
#include "string.h"
#include "intr.h"
#include "assert.h"
#include "x86.h"

#define PAGE_SIZE 4096

extern void intr_exit(void);
extern struct list thread_list_all;
extern struct list thread_ready_list;

void process_start(void* filename)
{
	void* function = filename;
	struct thread_struct* cur = running_thread();
    //char* sp = (char *)cur + PAGE_SIZE;
    //sp = sp - sizeof(struct trapframe);
    //struct trapframe* proc_stack = (struct trapframe *)sp;
	//cur->self_kstack += sizeof(struct thread_context);
	//struct trapframe* proc_stack = (struct trapframe *)cur->self_kstack;
    struct trapframe* proc_stack = cur->tf;
	proc_stack->edi = proc_stack->esi = proc_stack->ebp = proc_stack->esp_dummy = 0;
	proc_stack->ebx = proc_stack->edx = proc_stack->ecx = proc_stack->eax = 0;
	proc_stack->gs = 0;
	proc_stack->ds = proc_stack->es = proc_stack->fs = SELECTOR_U_DATA;
	proc_stack->eip = function;
	proc_stack->cs = SELECTOR_U_CODE;
	proc_stack->eflags = (EFLAGS_IOPL_0 | EFLAGS_MBS | EFLAGS_IF_1);
	proc_stack->esp = (void *)((uint32_t)get_a_page(PF_USER, USER_STACK3_VADDR) + PAGE_SIZE);
	proc_stack->ss = SELECTOR_U_DATA;
	asm volatile ("movl %0, %%esp; jmp intr_exit" : : "g" (proc_stack) : "memory");
}

void page_dir_activate(struct thread_struct* pthread)
{
	uint32_t pagedir_phy_addr;

	if (pthread->pgdir != NULL) {
		pagedir_phy_addr = addr_v2p((uint32_t)pthread->pgdir);
	} else {
        pagedir_phy_addr = 0x100000;
    }
	asm volatile ("movl %0, %%cr3" : : "r" (pagedir_phy_addr) : "memory");
}

void process_activate(struct thread_struct* pthread)
{
	page_dir_activate(pthread);
	if (pthread->pgdir) {
		update_tss_esp(pthread);
	}
}

uint32_t* create_page_dir(void)
{
    uint32_t* page_dir_vaddr = get_kernel_pages(1);
    if (page_dir_vaddr == NULL) {
        kprintf("create_page_dir: get_kernel_pages failed!\n");
        return NULL;
    }

    memcpy((uint32_t *)((uint32_t)page_dir_vaddr + 0x300 * 4), (uint32_t *)(0xfffff000 + 0x300 * 4), 1024);
    uint32_t new_page_dir_phy_addr = addr_v2p((uint32_t)page_dir_vaddr);
    page_dir_vaddr[1023] = new_page_dir_phy_addr | PG_US_U | PG_RW_W | PG_P_1;
    return page_dir_vaddr;
}

void create_user_vaddr_bitmap(struct thread_struct* user_prog)
{
    user_prog->userprog_vaddr.vaddr_start = USER_VADDR_START;
    uint32_t bitmap_pg_cnt = DIV_ROUND_UP((0xc0000000 - USER_VADDR_START)/PAGE_SIZE/8, PAGE_SIZE);
    user_prog->userprog_vaddr.vaddr_bitmap.bitmap_bits = get_kernel_pages(bitmap_pg_cnt);
    user_prog->userprog_vaddr.vaddr_bitmap.bitmap_len = (0xc0000000 - USER_VADDR_START)/PAGE_SIZE/8;
    bitmap_init(&user_prog->userprog_vaddr.vaddr_bitmap);
}

void process_execute(void* filename, char* name)
{
    struct thread_struct* thread = get_kernel_pages(1);
    Thread_init(thread, name, DEFAULT_PRIO);
    create_user_vaddr_bitmap(thread);
    Thread_create(thread, process_start, filename);
    thread->pgdir = create_page_dir();

    enum intr_status old_status = intr_disable();
    assert(!list_find(&thread_ready_list, &thread->general_tag));
    list_append(&thread_ready_list, &thread->general_tag);
    assert(!list_find(&thread_list_all, &thread->general_tag));
    list_append(&thread_list_all, &thread->all_list_tag);
    set_intr_status(old_status);
}
