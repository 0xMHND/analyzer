#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define BUF_SIZE 256
//#define format string at some point

//contains arguments of each ocrDbCreate() call
struct DbCreate_info {
	clock_t time;
	uint64_t db;
	void *addr;
	uint64_t len;
	uint16_t flags;
};

//contains arguments of each ocrDbDestroy() call
struct DbDestroy_info {
	clock_t time;
	uint64_t db;
};

//contains arguments of each ocrDbRelease() call
struct DbRelease_info {
	clock_t time;
	uint64_t db;
};

