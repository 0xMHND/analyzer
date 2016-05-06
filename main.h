#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>//for access function

#define LOGS_PATH "./logs/"
#define BUF_SIZE 256
#define DB_SIZE 4
#define READ 0
#define WRITE 1
#define RUN_NUM_DIGITS 3
#define MAX_NUM_ARGS 6

#define OCR_FUNCTIONS 1
#define OCR_FUNCTIONS_MAGIC "-<| OCR_FUNCTIONS |>-\n"
#define PROCESSED_DATA 2
#define PROCESSED_DATA_MAGIC "-<| PROCESSED_DATA |>-\n"
#define MEMORY_ANALYSIS 3
#define MEMORY_ANALYSIS_MAGIC "-<| MEMORY_ANALYSIS |>-\n"

char * LOG_PATH = NULL;

/******************* S T R U C T S *******************/

//contains arguments of each ocrDbCreate() call
struct dbc {
	uint64_t instr_count;	//instructions since beginning of program
	uint64_t depth;	//The depth level the ocrDbCreate was called at
	uint64_t id;		//datablock ID
	void *addr;		//datablock location in memory
	uint64_t len;		//datablock size
	uint16_t flags;		//datablock options
};

//contains arguments of each ocrDbDestroy() call
struct dbd {
	uint64_t instr_count;
	uint64_t id;
	uint64_t depth;	//The depth level the ocrDbDestroy was called at
};

//Contains all necessary variables for easy use of the previously defined structs
struct block_info 
{
	struct dbc * create;
	struct dbd * destroy;
	int c_count;		//count for the create[] array
	int d_count;		//count for the destroy[] array
	int size;		//actual size of both arrays
};

//Contains a double pointer for the lifetime_info, for random access of the blocks' pairs
struct lifetime_master{
	struct lifetime_info ** ptr;
	int size;
};

// Contain info of processed data for each block pair
struct lifetime_info{
	struct lifetime_info * partner;
	char *	 			   type;
	uint64_t 			   id;
	uint64_t 			   lifetime_instructions;
	double 	 			   lifetime_time ;
};

/******************* F U N C T I O N S *******************/
//not all included. functions headers in source file (.c)

void dbc_init (struct dbc create );
void dbd_init (struct dbd destroy);
void block_info_init (struct block_info * blocks);
