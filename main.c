#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datablock_parser.c"
#include "visualization.c"
#include "data_processing.c"


/***************************
 * input: 	struct to initialize
 * output:	none
 * description:	initializes the given struct by reference
***************************/
void dbc_init (struct dbc create)
{
	create.instr_count = -1;
	create.dbid = -1;
	create.addr = NULL;
	create.len  = 4;
	create.flags= 0;
}

/***************************
 * input: 	struct to initialize
 * output:	none
 * description:	initializes the given struct by reference
***************************/
void dbd_init(struct dbd destroy)
{
	destroy.instr_count = -1;
	destroy.dbid = 0;
}

/***************************
 * input: 	struct to initialize
 * output:	none
 * description:	initializes the given struct by reference
***************************/
void block_info_init( struct block_info * blocks) 
{
	blocks->size = 1;
	blocks->create  = malloc(blocks->size * sizeof(struct dbc));
	blocks->destroy = malloc(blocks->size * sizeof(struct dbd));
	blocks->c_count = 0;
	blocks->d_count = 0;

	dbc_init(blocks->create[0] );
	dbd_init(blocks->destroy[0]);
}

/***************************
 * input: 	number and contents of command line arguments
 * output:	return 0 on success
 * description:	runs all core blocks in correct order
***************************/
int main(int argc, char ** argv)
{
	struct block_info blocks;

	//exit if no filename is entered
	//or multiple are entered
	if ( argc != 2 ) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	block_info_init(&blocks);
	datablock_parser(&blocks, argv[1]);

	//printf statements for debugging
	printf("ocrDbCreate :");
	for (int i = 0; i < blocks.c_count; i++)
		printf(" %ld  ",blocks.create[i].instr_count);
	printf("\n");
	printf("ocrDbDestroy :");
	for (int i = 0; i < blocks.d_count; i++)
		printf(" %ld  ",blocks.destroy[i].instr_count);
	printf("\n");

	data_processing(&blocks);

	return 0;
}
