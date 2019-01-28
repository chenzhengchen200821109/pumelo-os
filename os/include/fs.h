#ifndef _FS_H
#define _FS_H

#define MAX_FILES_PER_PART 4096
#define BITS_PER_SECTOR 4096
#define SECTOR_SIZE 512
#define BLOCK_SIZE SECTOR_SIZE

enum file_type
{
	FT_UNKNOWN,
	FT_REGULAR,
	FT_DIRECTORY
};

#endif
