#include <stdlib.h>
#include <stdio.h>

#define DEF_SIZE 1

// ************** Feature 3 : Memory Usage ****************** // 
void memory_usage(struct block_info * block)
{

	uint64_t * x_instr = malloc((block->c_count + block->d_count) * sizeof(uint64_t) + 1);
	uint64_t * y_size = malloc((block->c_count + block->d_count) * sizeof(uint64_t) + 1);
	int ind=1;
	int i=0;
	int j=0;
	x_instr[0] = 0;
	y_size[0] = 0;
	for(; ind < block->c_count + block->d_count+1; ind++)
	{
		if ( j >= block->c_count )
			break;
		if ( block->destroy[i].instr_count == 0 )
			j = block->d_count;
		if ( (block->create[i].instr_count <= block->destroy[j].instr_count || block->destroy[j].instr_count == 0) && i < block->c_count) {
			x_instr[ind] = block->create[i].instr_count;
			y_size[ind] = y_size[ind-1] + DEF_SIZE;
			i++;
		}
		else if ( i < block->d_count) {
			x_instr[ind] = block->destroy[j].instr_count;
			y_size[ind] = y_size[ind-1] - DEF_SIZE;
			j++;
		}
	}
	plot_data(x_instr, y_size, ind);
}

void data_processing(struct block_info * block)
{
	printf("DATA PROCESSING .. \n");

// ************** Feature 1 : Data  block's lifetime ****************** // 
// Compute the difference between an ocrDbCreate and ocrDbDestory and print that. 
// For now the method is first created is first destroyed
// TODO: Compute the difference between ocrDbCreate and ocrDbDestroy that has the same GUID

	printf("[DataBlock's Lifetime] .. \n");
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
			printf("Block %d lifetime: %ld\n", i, block->destroy[j].instr_count - block->create[i].instr_count);
			j++;
		}
	}
	


// ************** Feature 3 : Memory Usage ****************** // 
	memory_usage(block);
} 

// ************** Feature 2 : Allocation Sizes ****************** // 
