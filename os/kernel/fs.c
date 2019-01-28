#include "fs.h"

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
	sb.part_lba_base = part->start->lba;

	sb.block_bitmap_lba = sb.part_lba_base + 2;
	sb.block_bitmap_sects = block_bitmap_sects;

	sb.inode_bitmap_lba = sb.block_bitmap_lba + sb.block_bitmap_sects;
	sb.inode_bitmap_sects = inode_bitmap_sects;

	sb.inode_table_lba = sb.inode_bitmap_lba + sb.inode_bitmap_sects;
	sb.inode_table_sects = inode_table_sects;

	sb.data_start_lba = sb.inode_table_lba + sb.inode_table_sects;
	sb.root_inode_no = 0;
	sb.dir_entry_size = sizeof(struct dir_entry);

	kprintf("%s info:\n", part->name);
	//kprintf("magic:0x%x\n part_lba_base:0x%x\n
	//				all_sectors:0x%x\n inode_cnt:0x%x\n
	//				block_bitmap_lba:0x%x\nblock_bitmap_sectos:0x%x\n
	//				inode_bitmap_lba:0x%x\n");
	
	struct disk* hd = part->my_disk;
	ide_write(hd, part->start_lba + 1, &sb, 1);
	kprintf("super_block_lba:0x%x\n", part->start_lba + 1);

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
	buf[0] != 0x01;
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

	kprintf("root_dir_lba: 0x%x\n", sb.data_start_lba);
	kpirntf("%s format done\n", part->name);
	sys_free(buf);

	
}
