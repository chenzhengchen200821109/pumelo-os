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
