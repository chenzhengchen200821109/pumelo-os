#include "bitmap.h"
#include <string.h>
#include <stdlib.h>
#include <error.h>
#include <assert.h>
#include <stdio.h>

#define WORD_TYPE           uint32_t
#define WORD_BITS           (sizeof(WORD_TYPE) * CHAR_BIT)
#define ON                   1
#define OFF                  0

// bitmap_create - allocate a new bitmap object.
struct bitmap *
bitmap_create(uint32_t nbits) {
    //static_assert(WORD_BITS != 0);
    assert(nbits != 0 && nbits + WORD_BITS > nbits);

    struct bitmap *bitmap;
    if ((bitmap = malloc(sizeof(struct bitmap))) == NULL) {
        return NULL;
    }

    uint32_t nwords = ROUNDUP_DIV(nbits, WORD_BITS);
    WORD_TYPE *map;
    if ((map = malloc(sizeof(WORD_TYPE) * nwords)) == NULL) {
        free(bitmap); // failure and release memory resource for bitmap
        return NULL;
    }

    bitmap->nbits = nbits, bitmap->nwords = nwords;
    bitmap->map = memset(map, 0xFF, sizeof(WORD_TYPE) * nwords);

    /* mark any leftover bits at the end in use(0) */
    if (nbits != nwords * WORD_BITS) {
        uint32_t ix = nwords - 1, overbits = nbits - ix * WORD_BITS;

        assert(nbits / WORD_BITS == ix);
        assert(overbits > 0 && overbits < WORD_BITS);

        for (; overbits < WORD_BITS; overbits++) {
            bitmap->map[ix] ^= (1 << overbits); /* xor */
        }
    }
    return bitmap;
}

int
bitmap_clear(struct bitmap *bitmap, uint32_t *index_store) {
    WORD_TYPE *map = bitmap->map;
    uint32_t ix, offset, nwords = bitmap->nwords;
    for (ix = 0; ix < nwords; ix++) {
        if (map[ix] != 0) { /* all zeros */
            for (offset = 0; offset < WORD_BITS; offset++) {
                WORD_TYPE mask = (1 << offset);
                if (map[ix] & mask) { /* find the first bit which is set */
                    map[ix] ^= mask; /* xor */
                    *index_store = ix * WORD_BITS + offset;
                    return 0;
                }
            }
            assert(0);
        }
    }
    return -1;
}

// bitmap_translate - according index, get the related word and mask
static void
bitmap_translate(struct bitmap *bitmap, uint32_t index, WORD_TYPE **word, WORD_TYPE *mask) {
    assert(index < bitmap->nbits);
    uint32_t ix = index / WORD_BITS, offset = index % WORD_BITS;
    *word = bitmap->map + ix;
    *mask = (1 << offset);
}

bool
bitmap_test(struct bitmap *bitmap, uint32_t index) {
    WORD_TYPE *word, mask;
    bitmap_translate(bitmap, index, &word, &mask);
    return (*word & mask);
}

// bitmap_set - according index, set related bit to 1
void
bitmap_set(struct bitmap *bitmap, uint32_t index) {
    WORD_TYPE *word, mask;
    bitmap_translate(bitmap, index, &word, &mask);
    assert(!(*word & mask));
    *word |= mask;
}

// bitmap_destroy - free memory contains bitmap
void
bitmap_destroy(struct bitmap *bitmap) {
    free(bitmap->map);
    free(bitmap);
}

// bitmap_getdata - return bitmap->map and the length of bits(words) to len_store
void *
bitmap_getdata(struct bitmap *bitmap, size_t *len_store) {
    if (len_store != NULL) {
        *len_store = sizeof(WORD_TYPE) * bitmap->nwords;
    }
    return bitmap->map;
}

// for debug
void bitmap_print(struct bitmap *bitmap)
{
    uint32_t* map = bitmap->map;
    uint32_t ix, offset, nwords = bitmap->nwords;

    printf("---print bitmap structure: begin---\n");
    printf("number of bits: %u\n", bitmap->nbits);
    printf("number of words: %u\n", bitmap->nwords);

    printf("each bit of map: ");
    for (ix = 0; ix < nwords; ix++) {
        for (offset = 0; offset < WORD_BITS; offset++) {
            uint32_t mask = (1 << offset);
            if (map[ix] & mask)
                printf("%d ", ON);
            else
                printf("%d ", OFF);
        }
        printf("\n");
    }
}
