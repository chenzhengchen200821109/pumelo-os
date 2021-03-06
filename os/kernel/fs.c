#include "fs.h"
#include "x86.h"
#include "ide.h"
#include "list.h"
#include "string.h"
#include "mem.h"
#include "bitmap.h"
#include "assert.h"
#include "stdio.h"

//#define MAX_FILE_NAME_LEN  16
#define MAX_FILES_PER_PART 4096
#define BITS_PER_SECTOR    4096
#define SECTOR_SIZE        512
#define BLOCK_SIZE         SECTOR_SIZE

static void partition_format(struct disk* hd, struct partition* part)
{
	uint32_t boot_sector_sects = 1;
	uint32_t super_block_sects = 1;
	uint32_t inode_bitmap_sects = DIV_ROUND_UP(MAX_FILES_PER_PART, BITS_PER_SECTOR);

	uint32_t inode_table_sects = DIV_ROUND_UP((sizeof(struct inode) * MAX_FILES_PER_PART), SECTOR_SIZE);
	uint32_t used_sects = boot_sector_sects + super_block_sects + inode_table_sects + inode_table_sects;
	uint32_t free_sects = part->sec_cnt = used_sects;

	uint32_t block_bitmap_sects;
	block_bitmap_sects = DIV_ROUND_UP(free_sects, BITS_PER_SECTOR);
	uint32_t block_bitmap_bit_len = free_sects - block_bitmap_sects;
	block_bitmap_sects = DIV_ROUND_UP(block_bitmap_bit_len, BITS_PER_SECTOR);

	struct super_block sb;
	sb.magic = 0x19590318;
	sb.sec_cnt = MAX_FILES_PER_PART;
	sb.part_lba_base = part->start_lba;

	sb.block_bitmap_lba = sb.part_lba_base + 2;
	sb.block_bitmap_sects = block_bitmap_sects;

	sb.inode_bitmap_lba = sb.block_bitmap_lba + sb.block_bitmap_sects;
	sb.inode_bitmap_sects = inode_bitmap_sects;

	sb.inode_table_lba = sb.inode_bitmap_lba + sb.inode_bitmap_sects;
	sb.inode_table_sects = inode_table_sects;

	sb.data_start_lba = sb.inode_table_lba + sb.inode_table_sects;
	sb.root_inode_no = 0;
	sb.dir_entry_size = sizeof(struct dir_entry);

	kprintf_lock("%s info:\n", part->name);
	kprintf_lock("	magic: 0x%x\n", sb.magic);
	kprintf_lock("	part_lba_base: 0x%x\n", sb.part_lba_base);
	kprintf_lock("	all_sectors: 0x%x\n", sb.sec_cnt);
	kprintf_lock("	inode_cnt: 0x%x\n", sb.inode_cnt);
	kprintf_lock("	block_bitmap_lba: 0x%x\n", sb.block_bitmap_lba);
	kprintf_lock("	block_bitmap_sectors: 0x%x\n", sb.block_bitmap_sects);
	kprintf_lock("	inode_bitmap_lba: 0x%x\n", sb.inode_bitmap_lba);
	kprintf_lock("  inode_bitmap_sectors: 0x%x\n", sb.inode_bitmap_sects);
	kprintf_lock("	inode_table_lba: 0x%x\n", sb.inode_table_lba);
	kprintf_lock("	inode_table_sectors: 0x%x\n", sb.inode_table_sects);
	kprintf_lock("	data_start_lba: 0x%x\n", sb.data_start_lba);
	
	//struct disk* hd = part->my_disk;
	ide_write(hd, part->start_lba + 1, &sb, 1);
	kprintf_lock("	super_block_lba: 0x%x\n", part->start_lba + 1);

	uint32_t buf_size = (sb.block_bitmap_sects >= sb.inode_bitmap_sects ? sb.block_bitmap_sects : sb.inode_bitmap_sects);
	buf_size = (buf_size >= sb.inode_table_sects ? buf_size : sb.inode_table_sects) * SECTOR_SIZE;

	uint8_t* buf = (uint8_t *)sys_malloc(buf_size);

	buf[0] |= 0x01;
	uint32_t block_bitmap_last_byte = block_bitmap_bit_len / 8;
	uint8_t block_bitmap_last_bit = block_bitmap_bit_len % 8;
	uint32_t last_size = SECTOR_SIZE - (block_bitmap_last_byte % SECTOR_SIZE);

	memset(&buf[block_bitmap_last_byte], 0xff, last_size);

	uint8_t bit_idx = 0;
	while (bit_idx <= block_bitmap_last_bit)
		buf[block_bitmap_last_byte] &= ~(1 << bit_idx++);

	ide_write(hd, sb.block_bitmap_lba, buf, sb.block_bitmap_sects);

	memset(buf, 0, buf_size);
	buf[0] |= 0x01;
	ide_write(hd, sb.block_bitmap_lba, buf, sb.inode_bitmap_sects);

	memset(buf, 0, buf_size);
	struct inode* i = (struct inode *)buf;
	i->i_size = sb.dir_entry_size * 2;
	i->i_no = 0;
	i->i_sectors[0] = sb.data_start_lba;
	ide_write(hd, sb.inode_table_lba, buf, sb.inode_table_sects);

	memset(buf, 0, buf_size);
	struct dir_entry* p_de = (struct dir_entry *)buf;

	memcpy(p_de->filename, ".", 1);
	p_de->i_no = 0;
	p_de->f_type = FT_DIRECTORY;
	p_de++;

	memcpy(p_de->filename, "..", 2);
	p_de->i_no = 0;
	p_de->f_type = FT_DIRECTORY;

	ide_write(hd, sb.data_start_lba, buf, 1);

	kprintf_lock("	root_dir_lba: 0x%x\n", sb.data_start_lba);
	kprintf_lock("%s format done\n", part->name);
	sys_free(buf);	
}

struct partition* cur_part;

static void mount_partition(struct list_entry* elem, int arg)
{
	char* part_name = (char *)arg;

	struct partition* part = to_struct(elem, struct partition, part_tag);
	if (!strcmp(part->name, part_name)) {
		cur_part = part;
		struct disk* hd = cur_part->my_disk;
		struct super_block* sb_buf = (struct super_block *)sys_malloc(SECTOR_SIZE);
		cur_part->sb = (struct super_block *)sys_malloc(sizeof(struct super_block));
		if (cur_part->sb == NULL)
			panic("alloc memory failed");
		memset(sb_buf, 0, SECTOR_SIZE);
		ide_read(hd, cur_part->start_lba + 1, sb_buf, 1);
		memcpy(cur_part->sb, sb_buf, sizeof(struct super_block));

		cur_part->block_bitmap.bitmap_bits = (uint8_t *)sys_malloc(sb_buf->block_bitmap_sects * SECTOR_SIZE);
		if (cur_part->block_bitmap.bitmap_bits == NULL)
			panic("alloc memory failed");
		cur_part->block_bitmap.bitmap_len = sb_buf->block_bitmap_sects * SECTOR_SIZE;

		ide_read(hd, sb_buf->block_bitmap_lba, cur_part->block_bitmap.bitmap_bits, sb_buf->block_bitmap_sects);

		cur_part->inode_bitmap.bitmap_bits = (uint8_t *)sys_malloc(sb_buf->inode_bitmap_sects * SECTOR_SIZE);
		if (cur_part->inode_bitmap.bitmap_bits == NULL)
			panic("alloc memory failed");
		cur_part->inode_bitmap.bitmap_len = sb_buf->inode_bitmap_sects * SECTOR_SIZE;
		ide_read(hd, sb_buf->inode_bitmap_lba, cur_part->inode_bitmap.bitmap_bits, sb_buf->inode_bitmap_sects);
		list_init(&cur_part->open_inodes);
		kprintf("mount %s done\n", part->name);
	}
}

extern uint8_t channel_cnt;
extern struct ide_channel channels[2];
extern struct list partition_list;

void filesys_init()
{
	uint8_t channel_no = 0, dev_no = 0, part_idx = 0;

	struct super_block* sb_buf = (struct super_block *)sys_malloc(SECTOR_SIZE);

	if (sb_buf == NULL) {
		panic("alloc memory failed");
	}
	kprintf_lock("searching filesystem...\n");
	while (channel_no < channel_cnt) {
		dev_no = 0;
		while (dev_no < 2) {
			if (dev_no == 0) {
				dev_no++;
				continue;
			}
			struct disk* hd = &channels[channel_no].devices[dev_no];
			struct partition* part = hd->prim_parts;
			while (part_idx < 12) {
				if (part_idx == 4)
					part = hd->logic_parts;
				if (part->sec_cnt != 0) {
					memset(sb_buf, 0, SECTOR_SIZE);
					ide_read(hd, part->start_lba + 1, sb_buf, 1);
					//if (sb_buf->magic == 0x19590318)
					//	kprintf_lock("%s has filesystem\n", part->name);
					//else {
						kprintf_lock("formating %s's partition %s......\n", hd->name, part->name);
						partition_format(hd, part);
					//}
				}
				part_idx++;
				part++;
			}
			dev_no++;
		}
		channel_no++;
	}
	sys_free(sb_buf);

	// mount partitions
	char default_part[8] = "sdb1";
	list_traversal(&partition_list, mount_partition, (int)default_part);
}


