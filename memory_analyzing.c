#include <stdlib.h>
#include <stdio.h>

#define MAX_MEMORY 100 

/***************************
 * input: block_info block that holds the informations about created and detroyed data blocks. 
 * output: N/A
 * description:	This funciton checks if the data blocks created are more than the destroyed 
 * 				which will indicate there is a memory leak or not. 
***************************/
void memory_leak(struct block_info *block)
{
	int ocrDbCreate = block->c_count;
	int ocrDbDestroy = block->d_count;
	int diff = ocrDbCreate-ocrDbDestroy;
	
	// Error check: if no data blocs are created or the passed block struct is corrupt. 	
	if(ocrDbCreate < 1)
		printf("There are no data blocks created!\n");

	// Check the number of the DataBlocks created and destroyed
	if(ocrDbCreate > ocrDbDestroy)
	{
		printf("There is a memory leak of %d Bytes\nThere should be %d extra ocrDbDestroy calls.\n", (diff)*DB_SIZE,diff);
		// prints the location of the blocks that got created but not destroyed 
		for(int i=0; i+diff-1<block->c_count;i++)
			printf("Memory leak happens at intrsuction number %ld\n", block->create[diff+i-1].instr_count);
	}
	else
		printf("No Memory Leaks\n");
}

/***************************
 * input: current memory used to check if exceeds maximum
 * output:	N/A
 * description:	Checks whether the current memory usage exceeded the maximum allowed
***************************/
void large_memory(int current_memory)
{
	//Error check : if the passed memory is negative.
	if(current_memory < 0)
		printf("ERROR: something is wrong, current memory is negative!\n");

	else if(current_memory > MAX_MEMORY)
		printf("WARNING!!: Alloactaing data blocks exceeded the maximum (100 bytes)\n");
}
