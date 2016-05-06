#include <stdlib.h>
#include <stdio.h>

#define F_HZ 50000000000.0
#define INSTR_PER_CYCLE 4.0
#define INSTR_PER_SEC F_HZ/INSTR_PER_CYCLE 

int find_create_index(struct block_info *, int);

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

	uint64_t * x_instr = malloc((block->c_count + block->d_count) * sizeof(uint64_t));
	uint64_t * y_size = malloc((block->c_count + block->d_count) * sizeof(uint64_t));
	int ind = 1;

	//Error handling for the number of calls for ocrDbCreate and ocrDbDestroy
	if(block->c_count < 0)
	{
		printf("Error: the number of created blocks is negative.\n");
		exit(1);
	}
	if(block->d_count < 0)
	{
		printf("Error: the number of destroyed blocks is negative.\n");
		exit(1);
	}
	// This for loop checks the next instructoin and whether it is ocrDbCreate or ocrDbDestroy then adds that to the x-axis
	// each time it does that it increments the y-axis if ocrDbCreate is called or decrements it if ocrDbDestroy is called
	x_instr[0] = block->create[0].instr_count;
	y_size[0] = block->create[0].len;;
	for(int i = 1, j = 0; i+j < block->c_count + block->d_count; )
	{
		// case1 : if the number od ocrDbDestroy exceeds ocrDbCreate 
//		if ( j >= block->c_count )
//			break;

		// case2 : ocrDbCreate is before ocrDbDestroy, Check the instruction count
		if ( (block->create[i].instr_count < block->destroy[j].instr_count) && i < block->c_count) {
			x_instr[ind] = block->create[i].instr_count;
			y_size[ind] = y_size[ind-1] + block->create[i].len;
			i++;
			ind++;
		}
		// case3 : ocrDbDestroy is before ocrDbCreate, Check the instruction count
		else if ( j < block->d_count) {
			int create_index = find_create_index(block, j);
			if((block->destroy[j].instr_count < block->create[i].instr_count || i >= block->c_count)){
				if (create_index != -1)	{
					x_instr[ind] = block->destroy[j].instr_count;
					y_size[ind] = y_size[ind-1] - block->create[create_index].len;
					ind++;
				} 
//else {
//					y_size[ind] = y_size[ind-1] - 4;
//				}
				j++;	
			}
		}
	}

	if (plot_data(x_instr, y_size, ind) != 0)
		printf("Error occurred while making output graph.\n");
}

// ************** Feature 1 : Data  block's lifetime ****************** // 
// Compute the difference between an ocrDbCreate and ocrDbDestory and print that. 
// For now the method is first created is first destroyed
//
// input: block_info block that holds the informations about created and detroyed data blocks. 
//
int find_create_index(struct block_info *block, int destroy_index){
	int create_index=-1;
	for(int i=0; i<block->c_count; i++){
		if(block->create[i].id == block->destroy[destroy_index].id)
		{
			create_index = i;
			break;
		}
	}
	return create_index;
}
int find_destroy_index(struct block_info *block, int create_index){
	int destroy_index=-1;
	for(int i=0; i<block->d_count; i++){
		if(block->create[create_index].id == block->destroy[i].id)
		{
			destroy_index = i;
			break;
		}
	}
	return destroy_index;
}


void db_lifetime(struct block_info *block)
{
	int j=0;

	struct lifetime_master * master = (struct lifetime_master*)malloc(sizeof(struct lifetime_master));
	struct lifetime_info ** ptr = (struct lifetime_info**)malloc(block->c_count*sizeof(struct lifetime_info));
	int cnt=0;
	//Error handling for the number of calls for ocrDbCreate and ocrDbDestroy
	if(block->c_count < 0)
	{
		printf("Error: the number of created blocks is negative.\n");
		exit(1);
	}
	if(block->d_count < 0)
	{
		printf("Error: the number of destroyed blocks is negative.\n");
		exit(1);
	}

	// loop through ocrDbCreate and match them with their ocrDbDestroy and compute the difference of instructions count. 
	for(int i=0;  i<block->c_count ;i++){
		//The case where we have more ocrDbCreate than destroy
		if(j>=block->d_count ){
			printf("Ran out of ocrDbDestroy\n");
			//loop through the remaining and print "inf" lifetime
			for(;i<block->c_count;i++)
				printf("Block %d lifetime: inf. \n", i);
		}
		else{
			int destroy_index = find_destroy_index(block, i);
			if(destroy_index == -1)
			{
//				printf("No Corresponding destroy found for block ID#%#lx\n", block->create[i].id);
				printf("Block #%d, ID#%#lx ", i, block->create[i].id);
				printf("lifetime: inf.\n");
			}
			else{
				struct lifetime_info * create = malloc(sizeof(struct lifetime_info));				
				struct lifetime_info * destroy = malloc(sizeof(struct lifetime_info));				
				double instructions_consumed = block->destroy[destroy_index].instr_count - block->create[i].instr_count;
				double time = (instructions_consumed / INSTR_PER_SEC);
				
				ptr[cnt] = create; cnt++;

				create->partner = destroy;
				destroy->partner = create;
				create->type =  "create";
				destroy->type =  "destroy";
				create->id = block->create[i].id;
				destroy->id = block->destroy[destroy_index].id;
				create->lifetime_instructions = instructions_consumed;
				destroy->lifetime_instructions = instructions_consumed;
				create->lifetime_time = time;
				destroy->lifetime_time = time;
				
				printf("Block #%d, ID#%#lx\t", i, block->create[i].id);
				printf("lifetime: %lf\t", (double)instructions_consumed);
				printf("instr. %lf" "seconds\n", (double)time);
				j++;
			}
		}
	}
	master->ptr = ptr;
	master->size = cnt;
	
	formatter_write((void*)master,PROCESSED_DATA);

	for(int i=0;i<cnt;i++)
	{
		free(ptr[i]->partner);
		free(ptr[i]);
	}
	free(master);
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

