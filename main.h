#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define BUF_SIZE 256

//contains arguments of each ocrDbCreate() call
struct dbc {
	uint64_t instr_count;
	uint64_t dbid;
	void *addr;
	uint64_t len;
	uint16_t flags;
};

//contains arguments of each ocrDbDestroy() call
struct dbd {
	uint64_t instr_count;
	uint64_t dbid;
};

//contains arguments of each ocrDbRelease() call
struct dbr {
	uint64_t instr_count;
	uint64_t dbid;
};



struct block_info 
{
	struct dbc * create;
	struct dbd * destroy;
	int c_count;
	int d_count;
	int size;
};

void dbc_init (struct dbc create );
void dbd_init (struct dbd destroy);
void block_info_init (struct block_info * blocks);
