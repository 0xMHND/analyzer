#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "datablock_parser.c"

void block_info_init( struct block_info * blocks) 
{
	blocks->size = DEF_SIZE;
	blocks->create  = malloc(blocks->size * sizeof(long));
	blocks->destroy = malloc(blocks->size * sizeof(long));
	blocks->c_count = 0;
	blocks->d_count = 0;
}

int main(int argc, char ** argv)
{
	struct block_info blocks;

	if ( argc != 2 ) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	block_info_init(&blocks);

	datablock_parser(&blocks, argv[1]);


	for (int i = 0; i < blocks.c_count; i++)
		printf("%ld  ",blocks.create[i]);
	printf("\n");
	for (int i = 0; i < blocks.d_count; i++)
		printf("%ld  ",blocks.destroy[i]);
	printf("\n");

	return 0;
}
