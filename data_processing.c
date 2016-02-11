#include <stdlib.h>
#include <stdio.h>

#define F_HZ 1000000000.0
#define INSTR_PER_CYCLE 4.0
#define INSTR_PER_SEC F_HZ/INSTR_PER_CYCLE 

// ************** Feature 3 : Memory Usage ****************** // 
// This function keeps track of memory with every ocrDbCreate and ocrDbDestroy
// Whenever a Data block is created we incerment the size array by the size of the DataBlock 
// also whenever an ocrDbDestory is called we decrement the size array by the size of the DataBlcok.
// All that while keeping track of the instructions count as we advances to the end.
// Then feed that back to the plot function as x-axis: instructon count
// 											   y-axis: Memory usage
//
//* input: block_info block that holds the informations about created and detroyed data blocks. 
void memory_usage(struct block_info * block)
{

	uint64_t * x_instr = malloc((block->c_count + block->d_count) * sizeof(uint64_t) + 1);
	uint64_t * y_size = malloc((block->c_count + block->d_count) * sizeof(uint64_t) + 1);
	int ind=1;
	int i=0;
	int j=0;
	x_instr[0] = 0;
	y_size[0] = 0;
	
	// This for loop checks the next instructoin and whether it is ocrDbCreate or ocrDbDestroy then adds that to the x-axis
	// each time it does that it increments the y-axis if ocrDbCreate is called or decrements it if ocrDbDestroy is called
	for(; ind < block->c_count + block->d_count+1; ind++)
	{
		// case1 : if the number od ocrDbDestroy exceeds ocrDbCreate 
		if ( j >= block->c_count )
			break;
		if ( block->destroy[i].instr_count == 0 )
			j = block->d_count;
		if ( (block->create[i].instr_count <= block->destroy[j].instr_count || block->destroy[j].instr_count == 0) && i < block->c_count) {
			x_instr[ind] = block->create[i].instr_count;
			y_size[ind] = y_size[ind-1] + DB_SIZE;
			i++;
		}
		else if ( i < block->d_count) {
			x_instr[ind] = block->destroy[j].instr_count;
			y_size[ind] = y_size[ind-1] - DB_SIZE;
			j++;
		}
	}
	plot_data(x_instr, y_size, ind);
}

// ************** Feature 1 : Data  block's lifetime ****************** // 
// Compute the difference between an ocrDbCreate and ocrDbDestory and print that. 
// For now the method is first created is first destroyed
// TODO: Compute the difference between ocrDbCreate and ocrDbDestroy that has the same GUID
// 
//* input: block_info block that holds the informations about created and detroyed data blocks. 
void db_lifetime(struct block_info *block)
{

	int j=0;
	for(int i=0;  i<block->c_count ;i++)
	{
		if(j>=block->d_count )
		{
			printf("Ran out of ocrDbDestroy\n");
			for(;i<block->c_count;i++)
			{
				printf("Block %d lifetime: inf. \n", i);
			}
		}
		else
		{
			printf("Block %d lifetime: %ld instr. %lf seconds\n", i, (block->destroy[j].instr_count-block->create[i].instr_count) , (block->destroy[j].instr_count-block->create[i].instr_count)/INSTR_PER_SEC);
			j++;
		}
	}
}


// ****************** main **********************//
// Main function that gets called in main.c
// this function basically calls the features' functions and pass them the block_info struct
//* input: block_info block that holds the informations about created and detroyed data blocks. 
void data_processing(struct block_info * block)
{
	printf("DATA PROCESSING .. \n");

// ************** Feature 1 : Data  block's lifetime ****************** // 
	printf("[DataBlock's Lifetime] .. \n");
	db_lifetime(block);	


// ************** Feature 3 : Memory Usage ****************** // 
	printf("[Memory Usage] .. \n");
	memory_usage(block);
} 

