#ifndef __BITMAP_H__
#define __BITMAP_H__

#include "defs.h"

/*
 * Fixed-size array of bits. (Intended for storage management.)
 *
 * Functions:
 *     bitmap_create  - create a new bitmap object given the number of bits and
 *                      set all bits on success. Otherwise returns NULL on error. 
 *     bitmap_getdata - return a pointer to raw bit data (for I/O).
 *     bitmap_clear   - clear the first bit which is set, and return its index.
 *     bitmap_test    - according index, return true its bit is set. Otherwise false.
 *     bitmap_set     - according index, set related bit to 1.
 *     bitmap_print   - on purpose of debug, print out information about the bitmap 
 *                      object.
 *     bitmap_destroy - destroy the bitmap object.
 */


//struct bitmap;

struct bitmap {
    uint32_t nbits;
    uint32_t nwords;
    uint32_t* map;
};

struct bitmap *bitmap_create(uint32_t nbits);                     
int bitmap_clear(struct bitmap *bitmap, uint32_t *index_store);   
bool bitmap_test(struct bitmap *bitmap, uint32_t index);         
void bitmap_set(struct bitmap *bitmap, uint32_t index);          
void bitmap_destroy(struct bitmap *bitmap);                       
void *bitmap_getdata(struct bitmap *bitmap, size_t *len_store);
void bitmap_print(struct bitmap *bitmap); // for debug

#endif /*__BITMAP_H__ */

