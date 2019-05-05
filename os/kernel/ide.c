#include "ide.h"
#include "string.h"
#include "assert.h"
#include "x86.h"
#include "stdio.h"

#define reg_data(channel) (channel->port_base + 0)
#define reg_error(channel) (channel->port_base + 1)
#define reg_sect_cnt(channel) (channel->port_base + 2)
#define reg_lba_l(channel) (channel->port_base + 3)
#define reg_lba_m(channel) (channel->port_base + 4)
#define reg_lba_h(channel) (channel->port_base + 5)
#define reg_dev(channel) (channel->port_base + 6)
#define reg_status(channel) (channel->port_base + 7)
#define reg_cmd(channel) (reg_status(channel))
#define reg_alt_status(channel) (channel->port_base + 0x206)
#define reg_ctl(channel) reg_alt_status(channel)

#define BIT_ALT_STAT_BSY    0x80
#define BIT_ALT_STAT_DRDY   0x40
#define BIT_ALT_STAT_DRQ    0x8
#define BIT_ALT_STAT_ERR    0x1

#define BIT_DEV_MBS         0xa0
#define BIT_DEV_LBA         0x40
#define BIT_DEV_DEV         0x10

#define CMD_IDENTIFY        0xec
#define CMD_READ_SECTOR     0x20
#define CMD_WRITE_SECTOR    0x30

#define IDE_ERR_BBK         0x80	// bad block
#define IDE_ERR_UNC         0x40	// uncorrectable data
#define IDE_ERR_MC          0x20	// media changed
#define IDE_ERR_IDNF        0x10	// ID mark not found
#define IDE_ERR_MCR         0x08	// media change request
#define IDE_ERR_ABRT        0x04	// command aborted
#define IDE_ERR_TK0NF       0x20	// track 0 not found
#define IDE_ERR_AMNF        0x01 	// no address mark

#define max_lba ((80 * 1024 * 1024 / 512) - 1)

int32_t ext_lba_base = 0;

// primary partition and logic partition
uint8_t p_no = 0, l_no = 0;

struct list partition_list;

uint8_t channel_cnt;
struct ide_channel channels[2];

static void select_disk(struct disk* hd)
{
	uint8_t reg_device = BIT_DEV_MBS | BIT_DEV_LBA;
	if (hd->dev_no == 1)
		reg_device |= BIT_DEV_DEV;
	outb(reg_dev(hd->my_channel), reg_device);
}

static void select_sector(struct disk* hd, uint32_t lba, uint8_t sec_cnt)
{
	assert(lba <= max_lba);
	assert(sec_cnt <= 255);
	struct ide_channel* channel = hd->my_channel;

	outb(reg_sect_cnt(channel), sec_cnt);

	//kprintf("lba = 0x%x\n", lba);
	outb(reg_lba_l(channel), lba);
	outb(reg_lba_m(channel), lba >> 8);
	outb(reg_lba_h(channel), lba >> 16);

	outb(reg_dev(channel), BIT_DEV_MBS | BIT_DEV_LBA | (hd->dev_no == 1 ? BIT_DEV_DEV : 0) | lba >> 24);
}

static void cmd_out(struct ide_channel* channel, uint8_t cmd)
{
	channel->expecting_intr = 1;
	outb(reg_cmd(channel), cmd);
}

static void read_from_sector(struct disk* hd, void* buf, uint8_t sec_cnt)
{
	uint32_t size_in_byte;

	if (sec_cnt == 0)
		size_in_byte = 256 * 512;
	else 
		size_in_byte = sec_cnt * 512;
	insw(reg_data(hd->my_channel), buf, size_in_byte / 2);
}

static void write_to_sector(struct disk* hd, void* buf, uint8_t sec_cnt)
{
	uint32_t size_in_byte;

	if (sec_cnt == 0)
		size_in_byte = 256 * 512;
	else
		size_in_byte = sec_cnt * 512;
	outsw(reg_data(hd->my_channel), buf, size_in_byte / 2);
}

static void check_ide_error(struct ide_channel* channel)
{
	uint8_t err;
	err = inb(reg_error(channel));
	if (err & IDE_ERR_BBK)
		kprintf_lock("Bad block\n");
	else if (err & IDE_ERR_UNC)
		kprintf_lock("Uncorrectable data\n");
	else if (err & IDE_ERR_MC)
		kprintf_lock("Media changed\n");
	else if (err & IDE_ERR_IDNF)
		kprintf_lock("ID mark not found\n");
	else if (err & IDE_ERR_MCR)
		kprintf_lock("Media change request\n");
	else if (err & IDE_ERR_ABRT)
		kprintf_lock("Command aborted\n");
	else if (err & IDE_ERR_TK0NF)
		kprintf_lock("Track 0 not found\n");
	else 
		kprintf_lock("No address mark\n");
}

static bool busy_wait(struct disk* hd)
{
	struct ide_channel* channel = hd->my_channel;
	uint8_t status;
	uint16_t time_limit = 30 * 1000;
	while (time_limit -= 10 > 0) {
		status = inb(reg_status(channel));
		//kprintf_lock("status = 0x%x\n", status);
		if (status & BIT_ALT_STAT_BSY) 
			mtime_sleep(10);
		else if (status & BIT_ALT_STAT_DRQ)
			return true;
		else if (status & BIT_ALT_STAT_ERR) {
			check_ide_error(channel);
			return false;
		}
	}
	return false;
}

static void swap_bytes(const char* dst, char* buf, uint32_t len)
{
	uint8_t idx;

	for (idx = 0; idx < len; idx += 2) {
		buf[idx + 1] = *dst++;
		buf[idx] = *dst++;
	}
	buf[idx] = '\0';
}

static void identify_disk(struct disk* hd)
{
	char id_info[512];
	select_disk(hd);
	cmd_out(hd->my_channel, CMD_IDENTIFY);

	// blocked thread
	sema_down(&hd->my_channel->disk_done);

	if (!busy_wait(hd)) {
		char error[64];
		snprintf(error, 64, "%s identify failed\n", hd->name);
		panic(error);
	}
	read_from_sector(hd, id_info, 1);

	char buf[64];
	uint8_t sn_start = 10 * 2, sn_len = 20, md_start = 27 * 2, md_len = 40;
	swap_bytes(&id_info[sn_start], buf, sn_len);
	kprintf("    disk %s info:\n", hd->name);
	kprintf("        SN: %s\n", buf);
	memset(buf, 0, sizeof(buf));
	swap_bytes(&id_info[md_start], buf, md_len);
	kprintf("        Module: %s\n", buf);
	uint32_t sectors = *(uint32_t *)&id_info[60 * 2];
	kprintf("        Sectors: %d\n", sectors);
	kprintf("        Capacity: %dMB\n", sectors * 512 / 1024 / 1024);
}

void ide_read(struct disk* hd, uint32_t lba, void* buf, uint32_t sec_cnt)
{
	assert(lba <= max_lba);
	assert(sec_cnt > 0);
	lock_acquire(&hd->my_channel->lock);

	// step 1
	select_disk(hd);

	uint32_t secs_op;
	uint32_t secs_done = 0;
	while (secs_done < sec_cnt) {
		if ((secs_done + 256) <= sec_cnt)
			secs_op = 256;
		else
			secs_op = sec_cnt - secs_done;
		// step 2
		select_sector(hd, lba + secs_done, secs_op);
		// step 3
		cmd_out(hd->my_channel, CMD_READ_SECTOR);

		sema_down(&hd->my_channel->disk_done);
		// step 4
		if (!busy_wait(hd)) {
			char error[64];
			snprintf(error, 64, "%s read sector %d failed\n", hd->name, lba);
			panic(error);
		}
		// step 5
		read_from_sector(hd, (void *)((uint32_t)buf + secs_done * 512), secs_op);
		secs_done += secs_op;
	}
	lock_release(&hd->my_channel->lock);
}

static void partition_scan(struct disk* hd, uint32_t ext_lba)
{
	struct boot_sector* bs = (struct boot_sector *)sys_malloc(sizeof(struct boot_sector));
	ide_read(hd, ext_lba, bs, 1);
	uint8_t part_idx = 0;
	struct partition_table_entry* p = bs->partition_table;

	while (part_idx < 4) {
		if (p->fs_type == 0x5) {  // extended partition
			if (ext_lba_base == 0) { // boot sector
				//hd->prim_parts[p_no].start_lba = ext_lba + p->start_lba;
				//hd->prim_parts[p_no].sec_cnt = p->sec_cnt;
				//hd->prim_parts[p_no].my_disk = hd;
				//list_append(&partition_list, &hd->prim_parts[p_no].part_tag);
				//snprintf(hd->prim_parts[p_no].name, 8, "%s%d", hd->name, p_no + 1);
				//kprintf_lock("extend partition name: %s\n", hd->prim_parts[p_no].name);
				//p_no++;
				//assert(p_no <= 4);
				// initial value of ext_lba;
				ext_lba_base = p->start_lba;
				partition_scan(hd, ext_lba_base);
			} else {
				//kprintf_lock("next ext_lba = 0x%x\n", ext_lba + p->start_lba);
				partition_scan(hd, ext_lba_base + p->start_lba);
				
			}
		} else if (p->fs_type == 0x83) {  // linux file system
			hd->prim_parts[p_no].start_lba = ext_lba + p->start_lba;
			hd->prim_parts[p_no].sec_cnt = p->sec_cnt;
			hd->prim_parts[p_no].my_disk = hd;
			list_append(&partition_list, &hd->prim_parts[p_no].part_tag);
			snprintf(hd->prim_parts[p_no].name, 8, "%s%d", hd->name, p_no + 1);
			//kprintf_lock("primary partition name: %s\n", hd->prim_parts[p_no].name);
			if (ext_lba_base == 0)
				p_no++;
			assert(p_no <= 4);
		} else if (p->fs_type == 0x0) {
			if (ext_lba_base == 0)
				p_no++;
			assert(p_no <= 4);
		} else if (p->fs_type == 0x66) {
			hd->logic_parts[l_no].start_lba = ext_lba + p->start_lba;
			hd->logic_parts[l_no].sec_cnt = p->sec_cnt;
			hd->logic_parts[l_no].my_disk = hd;
			list_append(&partition_list, &hd->logic_parts[l_no].part_tag);
			snprintf(hd->logic_parts[l_no].name, 8, "%s%d", hd->name, l_no + 5);
			//kprintf_lock("logic partition name: %s\n", hd->logic_parts[l_no].name);
			l_no++;
			if (l_no >= 8)
				return;
		}

		part_idx++;
		p++;
	}
	sys_free(bs);
}


//static void partition_scan(struct disk* hd, uint32_t ext_lba)
//{
//	struct boot_sector* bs = (struct boot_sector *)sys_malloc(sizeof(struct boot_sector));
//	ide_read(hd, ext_lba, bs, 1);
//	uint8_t part_idx = 0;
//	struct partition_table_entry* p = bs->partition_table;

//	while (part_idx++ < 4) {
//		if (p->fs_type == 0x5) { // extended partition
//			if (ext_lba_base != 0) {
//				partition_scan(hd, p->start_lba + ext_lba_base);
//			} else { // mbr
//				ext_lba_base = p->start_lba;
//				partition_scan(hd, p->start_lba);
//			}
//		} else if (p->fs_type != 0) { // linux
//			if (ext_lba == 0) {
//				hd->prim_parts[p_no].start_lba = ext_lba + p->start_lba;
//				hd->prim_parts[p_no].sec_cnt = p->sec_cnt;
//				hd->prim_parts[p_no].my_disk = hd;
//				list_append(&partition_list, &hd->prim_parts[p_no].part_tag);
//				snprintf(hd->prim_parts[p_no].name, 8, "%s%d", hd->name, p_no + 1);
//				kprintf("pimary partition name: %s\n", hd->prim_parts[p_no].name);
//				p_no++;
//				assert(p_no < 4);
//			} else {
//				hd->logic_parts[l_no].start_lba = ext_lba + p->start_lba;
//				hd->logic_parts[l_no].sec_cnt = p->sec_cnt;
//				hd->logic_parts[l_no].my_disk = hd;
//				list_append(&partition_list, &hd->logic_parts[l_no].part_tag);
//				snprintf(hd->logic_parts[l_no].name, 8, "%s%d", hd->name, l_no + 5);
//				l_no++;
//				if (l_no >= 8)
//					return;
//			}
//		}
//		p++;
//	}
//	sys_free(bs);
//}

static void partition_info(struct list_entry* elem, int arg)
{
	struct partition* part = to_struct(elem, struct partition, part_tag);
	kprintf_lock("%s start_lba: 0x%x, sec_cnt:0x%x\n", part->name, part->start_lba, part->sec_cnt);
}


void ide_write(struct disk* hd, uint32_t lba, void* buf, uint32_t sec_cnt)
{
	assert(lba <= max_lba);
	assert(sec_cnt > 0);
	lock_acquire(&hd->my_channel->lock);

	select_disk(hd);

	uint32_t secs_op;
	uint32_t secs_done = 0;
	while (secs_done < sec_cnt) {
		if ((secs_done + 256) <= sec_cnt)
			secs_op = 256;
		else
			secs_op = sec_cnt - secs_done;

		select_sector(hd, lba + secs_done, secs_op);

		cmd_out(hd->my_channel, CMD_WRITE_SECTOR);

		if (!busy_wait(hd)) {
			char error[64];
			snprintf(error, 64, "%s write sector %d failed\n", hd->name, lba);
			panic(error);
		}

		write_to_sector(hd, (void *)((uint32_t)buf + secs_done * 512), secs_op);

		sema_down(&hd->my_channel->disk_done);
		secs_done += secs_op;
	}
	lock_release(&hd->my_channel->lock);
}

void hd_intr_handler(uint8_t irq_no)
{
	uint8_t res;
	assert(irq_no == 0x2e || irq_no == 0x2f);
	uint8_t channel_no = irq_no - 0x2e;
	struct ide_channel* channel = &channels[channel_no];
	assert(channel->irq_no == irq_no);
	if (channel->expecting_intr) {
		channel->expecting_intr = 0;
		sema_up(&channel->disk_done);
		res = inb(reg_status(channel));
		//kprintf_lock("status = 0x%x\n", res);
	}
}

void ide_init()
{
	kprintf("ide_init start\n");
	uint8_t hd_cnt = *(uint8_t *)0x475;
	assert(hd_cnt > 0);

	channel_cnt = DIV_ROUND_UP(hd_cnt, 2);

	struct ide_channel* channel;
	uint8_t channel_no = 0;
	uint8_t dev_no = 0;

	while (channel_no < channel_cnt) {
		channel = &channels[channel_no];
		snprintf(channel->name, sizeof(channel->name), "ide%d", channel_no);

		switch(channel_no) {
			case 0:
				channel->port_base = 0x1f0;
				channel->irq_no = 0x20 + 14;
				break;
			case 1:
				channel->port_base = 0x170;
				channel->irq_no = 0x20 + 15;
				break;
		}
		// MUST INIT before ACCESS it
		list_init(&partition_list);
		channel->expecting_intr = 0;
		lock_init(&channel->lock);

		sema_init(&channel->disk_done, 0);

		while (dev_no < 2) {
			struct disk* hd = &channel->devices[dev_no];
			hd->my_channel = channel;
			hd->dev_no = dev_no;
			snprintf(hd->name, 8, "sd%c", 'a' + channel_no * 2 + dev_no);
			identify_disk(hd);
			if (dev_no != 0)
				partition_scan(hd, 0);
			p_no = 0;
			l_no = 0;
			dev_no++;
		}
		channel_no++;
	}

	kprintf("all partition info\n");
	list_traversal(&partition_list, partition_info, (int)NULL);
	kprintf_lock("length is %d\n", list_len(&partition_list));
	kprintf("ide_init done\n");
}

