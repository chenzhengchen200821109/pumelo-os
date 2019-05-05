#ifndef _FS_H
#define _FS_H

#include "defs.h"

#define MAX_FILE_NAME_LEN  16
//#define MAX_FILES_PER_PART 4096
//#define BITS_PER_SECTOR    4096
//#define SECTOR_SIZE        512
//#define BLOCK_SIZE         SECTOR_SIZE

// super block
struct super_block
{
	uint32_t magic;
	uint32_t sec_cnt;
	uint32_t inode_cnt;
	uint32_t part_lba_base;

	uint32_t block_bitmap_lba;
	uint32_t block_bitmap_sects;

	uint32_t inode_bitmap_lba;
	uint32_t inode_bitmap_sects;

	uint32_t inode_table_lba;
	uint32_t inode_table_sects;

	uint32_t data_start_lba;
	uint32_t root_inode_no;
	uint32_t dir_entry_size;

	uint8_t pad[460];
} __attribute__((packed));

//struct inode
//{
//	uint32_t i_no;
//	uint32_t i_size;
//	uint32_t i_open_cnts;
//	bool write_deny;
//	uint32_t i_sectors[13];
//	struct list_entry inode_tag;
//};

struct dir
{
	struct inode* inode;
	uint32_t dir_pos;
	uint8_t dir_buf[512];
};

enum file_type
{
	FT_UNKNOWN,
	FT_REGULAR,
	FT_DIRECTORY
};

struct dir_entry
{
	char filename[MAX_FILE_NAME_LEN];
	uint32_t i_no;
	enum file_type f_type;
};

void filesys_init();
#endif
