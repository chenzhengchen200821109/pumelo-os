#ifndef _LIST_H__
#define _LIST_H__

#include <stdio.h>
#include <stdint.h>

/* *
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when manipulating
 * whole lists rather than single entries, as sometimes we already know
 * the next/prev entries and we can generate better code by using them
 * directly rather than using the generic single-entry routines.
 * */


struct list_entry {
    struct list_entry *prev, *next;
};

struct list
{
    struct list_entry header;
    struct list_entry tailer;
};

typedef struct list_entry list_entry_t;
typedef void (*function)(list_entry_t* elem, int arg);

void list_init(struct list* plist);
void list_insert_before(list_entry_t* before, list_entry_t* elem);
void list_push(struct list* plist, list_entry_t* elem);
//void list_iterate(struct list* plist);
void list_append(struct list* plist, list_entry_t* elem);
void list_remove(struct list_entry* elem);
list_entry_t* list_pop(struct list* plist);
int list_empty(struct list* plist);
uint32_t list_len(struct list* plist);
void list_traversal(struct list* plist, function func, int arg);
int list_find(struct list* plist, list_entry_t* elem);
void list_print(struct list_entry* elem, int value);

#endif /* _LIST_H__ */

