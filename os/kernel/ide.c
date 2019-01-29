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

#define BIT_DEV_MBS         0xa
#define BIT_DEV_LBA         0x40
#define BIT_DEV_DEV         0x10

#define CMD_IDENTIFY        0xec
#define CMD_READ_SECTOR     0x20
#define CMD_WRITE_SECTOR    0x30

#define max_lba ((80 * 1024 * 1024 / 512) - 1)

int32_t ext_lba_base = 0;

uint8_t p_no = 0, l_no = 0;

struct list partition_list;

struct partition_table_entry
{
	uint8_t bootable;
	uint8_t start_head;
	uint8_t start_sec;
	uint8_t start_chs;
	uint8_t fs_type;
	uint8_t end_head;
	uint8_t end_sec;
	uint8_t end_chs;
	uint32_t start_lba;
	uint32_t sec_cnt;
} __attribute__((packed));

struct boot_sector
{
	uint8_t other[464];
	struct partition_table_entry partition_table[4];
	uint16_t signature;
} __attribute__((packed));

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
	struct ide_channel* channel = hd->my_channel;

	outb(reg_sect_cnt(channel), sec_cnt);

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
	insl(reg_data(hd->my_channel), buf, size_in_byte / 2);
}

static void write_to_sector(struct disk* hd, void* buf, uint8_t sec_cnt)
{
	uint32_t size_in_byte;

	if (sec_cnt == 0)
		size_in_byte = 256 * 512;
	else
		size_in_byte = sec_cnt * 512;
	outsl(reg_data(hd->my_channel), buf, size_in_byte / 2);
}

static int busy_wait(struct disk* hd)
{
	struct ide_channel* channel = hd->my_channel;
	uint16_t time_limit = 30 * 1000;
	while (time_limit -= 10 > 0) {
		if (!inb(reg_status(channel)) & BIT_ALT_STAT_BSY)
			return (inb(reg_status(channel)) & BIT_ALT_STAT_DRQ);
		else
			mtime_sleep(10);
	}
	return 0;
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
	kprintf("disk %s info:\n sn: %s\n", hd->name, buf);
	memset(buf, 0, sizeof(buf));
	swap_bytes(&id_info[md_start], buf, md_len);
	kprintf("module: %s\n", buf);
	uint32_t sectors = *(uint32_t *)&id_info[60 * 2];
	kprintf("sectors: %d\n", sectors);
	kprintf("capacity: %dMB\n", sectors * 512 / 1024 / 1024);
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
	

		select_sector(hd, lba + secs_done, secs_op);

		cmd_out(hd->my_channel, CMD_READ_SECTOR);

		sema_down(&hd->my_channel->disk_done);

		if (!busy_wait(hd)) {
			char error[64];
			snprintf(error, 64, "%s read sector %d failed\n", hd->name, lba);
			panic(error);
		}

		read_from_sector(hd, (void *)((uint32_t)buf + secs_done * 512), secs_op);
		secs_done += secs_op;
	}
	lock_release(&hd->my_channel->lock);
}


static void partition_scan(struct disk* hd, uint32_t ext_lba)
{
	//struct boot_sector* bs = sys_malloc(sizeof(struct boot_sector));
	char stack[512];
	struct boot_sector* bs = (struct boot_sector *)stack;
	ide_read(hd, ext_lba, bs, 1);
	uint8_t part_idx = 0;
	struct partition_table_entry* p = bs->partition_table;

	while (part_idx++ < 4) {
		if (p->fs_type == 0x5) {
			if (ext_lba_base != 0) {
				partition_scan(hd, p->start_lba + ext_lba_base);
			} else {
				ext_lba_base = p->start_lba;
				partition_scan(hd, p->start_lba);
			}
		} else if (p->fs_type != 0) {
			if (ext_lba == 0) {
				hd->prim_parts[p_no].start_lba = ext_lba + p->start_lba;
				hd->prim_parts[p_no].sec_cnt = p->sec_cnt;
				hd->prim_parts[p_no].my_disk = hd;
				list_append(&partition_list, &hd->prim_parts[p_no].part_tag);
				snprintf(hd->prim_parts[p_no].name, 8, "%s%d", hd->name, p_no + 1);
				p_no++;
				assert(p_no < 4);
			} else {
				hd->logic_parts[l_no].start_lba = ext_lba + p->start_lba;
				hd->logic_parts[l_no].sec_cnt = p->sec_cnt;
				hd->logic_parts[l_no].my_disk = hd;
				list_append(&partition_list, &hd->logic_parts[l_no].part_tag);
				snprintf(hd->logic_parts[l_no].name, 8, "%s%d", hd->name, l_no + 5);
				l_no++;
				if (l_no >= 8)
					return;
			}
		}
		p++;
	}
	//sys_free(bs);
}

static int partition_info(struct list_entry* elem, int arg)
{
	struct partition* part = to_struct(elem, struct partition, part_tag);
	kprintf("%s start_lba: 0x%x, secn_cnt:0x%x\n", part->name, part->start_lba, part->sec_cnt);

	return 0;
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
	assert(irq_no == 0x2e || irq_no == 0x2f);
	uint8_t channel_no = irq_no - 0x2e;
	struct ide_channel* channel = &channels[channel_no];
	assert(channel->irq_no == irq_no);
	if (channel->expecting_intr) {
		channel->expecting_intr = 0;
		sema_up(&channel->disk_done);
		inb(reg_status(channel));
	}
}

void ide_init()
{
	kprintf("ide_init start\n");
	uint8_t hd_cnt = *(uint8_t *)0x475;
	assert(hd_cnt > 0);

	//channel_cnt = DIV_ROUND_UP(hd_cnt, 2);
	channel_cnt = hd_cnt / 2;

	struct ide_channel* channel;
	uint8_t channel_no = 0;
	uint8_t dev_no = 0;

	while (channel_no < channel_cnt) {
		channel = &channels[channel_no];
		snprintf(channel->name, sizeof(channel->name), "ide%d", channel_no);
		//kprintf("%s\n", channel->name);

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

	//channel_no = 0;
	//while (channel_no < channel_cnt) {
	//	register_handler(channel->irq_no, hd_intr_handler);
	//	channel_no++;
	//}

	//while (dev_no < 2) {
	//	struct disk* hd = &channel->devices[dev_no];
	//	hd->my_channel = channel;
	//	hd->dev_no = dev_no;
	//	snprintf(hd->name, 8, "sd%c", 'a' + channel_no * 2 + dev_no);
	//	identify_disk(hd);
	//	if (dev_no != 0)
	//		partition_scan(hd, 0);
	//	p_no = 0;
	//	l_no = 0;
	//}
	//dev_no = 0;
	//channel_no++;

	kprintf("\nall partition info\n");
	list_traversal(&partition_list, partition_info, (int)NULL);
	kprintf("ide_init done\n");
}




