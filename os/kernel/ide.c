#include "ide.h"

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

uint8_t channel_cnt;
struct ide_channel channel[2];

static void select_disk(struct disk* hd)
{
	uint8_t reg_device = BIT_DEV_MBS | BIT_DEV_LBA;
	if (hd->dev_no == 1)
		reg_device |= BIT_DEV_DEV;
	outb(reg_dev(hd->my_channel), reg_devices);
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
	channel->expecting_intr = true;
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

static int busy_wait(struct disk* hd)
{
	struct ide_channel* channel = hd->my_channel;
	uint16_t time_limit = 30 * 1000;
	while (time_limit -= 10 > 0) {
		if (!inb(reg_status(channel)) & BIT_STAT_BSY)
			return (inb(reg_status(channel)) & BIT_STAT_DRQ);
		else
			mtime_sleep(10);
	}
	return false;
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
			sprintf(error, "%s read sector %d failed\n", hd->name, lba);
			panic(error);
		}

		read_from_sector(hd, (void *)((uint32_t)buf + secs_done * 512), secs_op);
		secs_done += secs_op;
	}
	lock_release(&hd->my_channel->lock);
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
			sprintf(error, "%s write sector %d failed\n", hd->name, lba);
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
	uint8_t ch_no = irq_no - 0x2e;
	struct ide_channel* channel = &channels[channel_no];
	assert(channel->irq_no == irq_no);
	if (channel->expecting_intr) {
		channel->expecting_intr = false;
		sema_up(&channel->disk_done);
		inb(reg_status(channel);
	}
}

void ide_init()
{
	kprintf("ide_init start\n");
	uint8_t hd_cnt = *(uint8_t *)0x475;
	assert(hd_cnt > 0);

	channel_cnt = DIV_ROUND_UP(hd_cnt);
	uint8_t channel_no = 0;

	while (channel_no < channel_cnt) {
		channel = &channels[channel_no];
		sprintf(channel->name, "ide%d", channel_no);

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
		channel->expecting_intr = false;
		lock_init(&channel->lock);

		sema_init(&channel->disk_done, 0);
		channel_no++;
	}

	while (channel_no < channel_cnt) {
		register_handler(channel->irq_no, hd_intr_handler);
		channel_no++;
	}

	kprintf("ide_init done\n");
}

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

static void swap_bytes(const char* dst, char* buf, uint32_t len)
{
	uint8_t idx;

	for (idx = 0; idx < len; idx += 2) {
		buf[idx + 1] = *dst++;
		buf[idx] = *dst++;
	}
	buf[idx] = '\0';
}

static void identify_disk(struct disk* disk)
{
	char id_info[512];
	select_disk(hd);
	cmd_out(hd->my_channel, CMD_IDENTIFY);

	sema_down(&hd->my_channel->disk_done);

	if (!busy_wait(hd)) {
		char error[64];
		sprintf(error, "%s identify failed\n", hd->name);
		panic(error);
	}
	read_from_sector(hd, id_info, 1);

	char buf[64];
	uint8_t sn_start = 10 * 2, sn_len = 20, md_start = 27 * 2, md_len = 40;
	swap_bytes(&id_info[sn_start], buf, sn_len);
	kprintf("disk %s info:\n sn: %s\n", hd->name, buf);
	memset(buf, 0, sizeof(buf));
	swap_bytes(&id_info[md_start], buf, md_len);
	kprintf("module: %s\n"， buf);
	uint32_t sectors = *(uint32_t *)&id_info[60 * 2];
	kprintf("sectors: %d\n", sectors);
	kprintf("capacity: %dMB\n", sectors * 512 / 1024 / 1024);
}