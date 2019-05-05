#include "thread.h"
#include "tss.h"
#include "osdefs.h"

extern void intr_exit(void);

void process_start(void* filename)
{
	void* function = filename;
	struct thread_struct* cur = running_thread();
	cur->self_kstack += sizeof(struct thread_struct);
	struct intr_stack* proc_stack = (struct intr_stack *)cur->self_kstack;
	proc_stack->edi = proc_stack->esi = proc_stack->ebp = proc_stack->esp_dummy = 0;
	proc_stack->ebx = proc_stack->edx = proc_stack->ecx = proc_stack->eax = 0;
	proc_stack->gs = 0;
	proc_stack->ds = proc_stack->es = proc_stack->fs = SELECTOR_U_DATA;
	proc_stack->eip = function;
	proc_stack->cs = SELECTOR_U_CODE;
	proc_stack->eflags = (EFLAGS_IOPL_0 | EFLAGS_MBS | EFLAGS_IF_l);
	proc_stack->esp = (void *)((uint32_t)get_a_page(PF_USER, USER_STACK3_VADDR) + PAGE_SIZE);
	proc_stack->ss = SELECTOR_U_DATA;
	asm volatile ("movl %0, %%esp; jmp intr_exit" : : "g" (proc_stack) : "memory");
}

void page_dir_activate(struct thread_struct* pthread)
{
	uint32_t pagedir_phy_addr = 0x100000;
	if (pthread->pgdir != NULL) {
		pagedir_phy_addr = addr_v2p((uint32_t)pthread->pgdir);
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
