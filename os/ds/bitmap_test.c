#include "bitmap.h"
#include <stdio.h>

int main()
{
    struct bitmap* pbm;
    uint32_t index;

    pbm = bitmap_create(31);
    bitmap_print(pbm);

    bitmap_clear(pbm, &index);
    printf("index = %u\n", index);
    bitmap_clear(pbm, &index);
    printf("index = %u\n", index); 
    bitmap_clear(pbm, &index);
    printf("index = %u\n", index);
    bitmap_print(pbm);

    bitmap_set(pbm, 2);
    bitmap_print(pbm);

    printf("%d\n", bitmap_test(pbm, 30));
    printf("%d\n", bitmap_test(pbm, 0));

    bitmap_destroy(pbm);
    return 0;
}
