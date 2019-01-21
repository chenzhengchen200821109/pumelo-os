#include "list.h"
#include <stdio.h>
#include "intr.h"
#include "thread.h"
#include "defs.h"

void list_init(struct list* plist)
{
    plist->header.prev = NULL;
    plist->header.next = &plist->tailer;
    plist->tailer.prev = &plist->header;
    plist->tailer.next = NULL;
}

void list_insert_before(struct list_entry* before, struct list_entry* elem)
{
    enum intr_status old_status = intr_disable();
    before->prev->next = elem;
    elem->prev = before->prev;
    elem->next = before;
    before->prev = elem;
    set_intr_status(old_status);
}

void list_push(struct list* plist, struct list_entry* elem)
{
    list_insert_before(plist->header.next, elem);
}

void list_append(struct list* plist, struct list_entry* elem)
{
    list_insert_before(&plist->tailer, elem);
}

void list_remove(struct list_entry* elem)
{
    enum intr_status old_status = intr_disable();
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    set_intr_status(old_status);
}

struct list_entry* list_pop(struct list* plist)
{
    struct list_entry* elem = plist->header.next;
    list_remove(elem);
    return elem;
}

int list_find(struct list* plist, struct list_entry* target)
{
    struct list_entry* elem = (plist->header).next;
    while (elem != &plist->tailer) {
        kprintf("elem = 0x%x, target = 0x%x\n", elem, target);
        if (elem == target)
            return 1;  // true
        elem = elem->next;
    }
    return 0;  // false
}

uint32_t list_len(struct list* plist)
{
    struct list_entry* first = plist->header.next;
    uint32_t counter = 0;
    while (first != &plist->tailer) {
        counter++;
        first = first->next;
    }
    return counter;
}

int list_empty(struct list* plist)
{
    return (plist->header.next == &plist->tailer ? 1 : 0);
}

void list_traversal(struct list* plist, function func, int arg)
{
    struct list_entry* first = plist->header.next;
    if (list_empty(plist))
        return;

    while (first != &plist->tailer) {
        func(first, arg);
        first = first->next;
    }
}

void list_print(struct list_entry* elem, int value)
{
    kprintf("addr 0x%x\n", to_struct(elem, struct thread_struct, general_tag));
}
