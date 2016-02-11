#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void datablock_parser(struct block_info * blocks, char * filename)
{
	uint64_t count = 0, depth_level;
	FILE * f;
	char buf[BUF_SIZE];
	char * func_name;

	//Open the OCR functions file for reading
	if ( (f = fopen( filename, "r")) == NULL ) {
		perror("Cannot open file specified");
		exit(1);
	}

	//For every iteration, read the next line in the file and parse the
	//line into tokens. If an ocrDb function is located, record the number of instructions
	//that have been executed since the program started. If a function is returned, add it's
	//number of executed instructions to the instruction count
	while(!feof(f)) {

		
		memset ( buf, '\0', BUF_SIZE);		//clear buffer
		if (fgets( buf, BUF_SIZE, f) == NULL)	//get the next line
			break;

		if (strtok( buf, " /|\n\t\r\\") == NULL)//start token parsing
			break;

		strtol(strtok(NULL, " /|\n\t\r\\"), NULL, 0);		//parse program counter

		depth_level = strtol(strtok(NULL, " /|\n\t\r\\"), NULL, 10);		//parse depth level

		func_name = strtok(NULL, " /|\n\t\r\\");	//parse function name

		//if a function is returning, add num of instructions to the count
		if (!strcmp(func_name, "returned.")) {
			count += atoi(strtok(NULL, " /|\n\t\r\\"));
		}
		//else if ocrDbCreate() is located, record the number of instructions that have
		//passed since the beginning of the program
		else if (!strcmp(func_name, "ocrDbCreate()")) {

			//if the array is too small, double its size
			if (blocks->c_count >= blocks->size) {
				blocks->create  = realloc(blocks->create , blocks->size * 2 * sizeof(struct dbc));
				blocks->destroy = realloc(blocks->destroy, blocks->size * 2 * sizeof(struct dbd));
				for (int i = blocks->size; i < blocks->size * 2; i++) {
					dbc_init(blocks->create[i] );
					dbd_init(blocks->destroy[i]);
				} 
				blocks->size *= 2;
			}

			blocks->create[blocks->c_count].instr_count = count;
			blocks->create[blocks->c_count].depth = depth_level;
			blocks->c_count++;
		}
		//else if ocrDbDestroy() is located, record the number of instructions that have
		//passed since the beginning of the program
		else if (!strcmp(func_name, "ocrDbDestroy()")) {

			//if the array is too small, double its size
			if (blocks->d_count >= blocks->size) {
				blocks->create  = realloc(blocks->create , blocks->size * 2 * sizeof(struct dbc));
				blocks->destroy = realloc(blocks->destroy, blocks->size * 2 * sizeof(struct dbd));
				for (int i = blocks->size; i < blocks->size * 2; i++) {
					dbc_init(blocks->create[i] );
					dbd_init(blocks->destroy[i]);
				} 
				blocks->size *= 2;
			}

			blocks->destroy[blocks->d_count].instr_count = count;
			blocks->destroy[blocks->d_count].depth = depth_level;
			blocks->d_count++;
		}
	}
}
