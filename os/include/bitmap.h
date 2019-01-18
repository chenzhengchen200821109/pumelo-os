#ifndef _BITMAP_H
#define _BITMAP_H

//#include "defs.h"
#include <stdint.h>
#define BITMAP_MASK 1

struct bitmap
{
    uint32_t bitmap_len;
    uint8_t* bitmap_bits;
};

void bitmap_init(struct bitmap* btmp);
int bitmap_scan_test(struct bitmap* btmp, uint32_t bit_idx);
int bitmap_scan(struct bitmap* btmp, uint32_t cnt);
void bitmap_set(struct bitmap* btmp, uint32_t bit_idx, int8_t value);

#endif /* _BITMAP_H */

