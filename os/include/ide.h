#ifndef _IDE_H
#define _IDE_H

#include "defs.h"
#include "bitmap.h"

struct partition
{
    uint32_t start_lba;
    uint32_t sec_cnt;
    struct disk* my_disk;
    struct list_elem part_tag;
    char name[8];
    struct super_block* sb;
    struct bitmap block_bitmap;
    struct bitmap inode_bitmap;
    struct list open_inodes;
};

struct disk
{
    char name[8];
    struct ide_channel* my_channel;
    uint8_t dev_no;
    struct partition prim_parts[4];
    struct partition logic_parts[8];
};

struct ide_channel
{
    char name[8];
    uint16_t port_base;
    uint8_t irq_no;
    struct lock lock;
    int expecting_intr;
    struct semaphore disk_done;
    struct disk devices[2];
};

#endif
