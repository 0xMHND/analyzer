#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/***************************
 * input: 	a struct pointer that will contain parsed data
 * 			a string containing the path to the file to be parsed
 * output:	none
 * description:	This function opens an OCR functions file and takes out
 * 				all the function calls relating to memory usage and saves that data
***************************/
void datablock_parser(struct block_info * blocks, char * filename)
{
	uint64_t count = 0, depth_level, prog_cnt;
	FILE * f;
	char buf[BUF_SIZE];
	char * func_name, *temp;
	uint64_t args[MAX_NUM_ARGS];

	//Open the OCR functions file for reading
	if ( (f = fopen( filename, "r")) == NULL ) {
		perror("Cannot open file specified");
		exit(EXIT_FAILURE);
	}

	//For every iteration, read the next line in the file and parse the
	//line into tokens. If an ocrDb function is located, record the number of instructions
	//that have been executed since the program started. If a function is returned, add it's
	//number of executed instructions to the instruction count
	while(!feof(f)) {
		
		memset ( buf, '\0', BUF_SIZE);		//clear buffer
		fgets(buf, BUF_SIZE, f);
		if (ferror(f)) {
			fprintf(stderr,"Could not read from file %s: %s.\n", filename, strerror(errno));
			exit(EXIT_FAILURE);
		}

		//if there is only whitespace, work done. break out of loop
		if ((func_name = strtok(buf, " /|\n\t\r\\")) == NULL)
			break;
		//if the first parsed value is nonzero, save the following info
		if (strtol(func_name, NULL, 10) != 0) {

			prog_cnt = strtol(strtok(NULL, " /|\n\t\r\\"), NULL, 0);		//parse program counter

			depth_level = strtol(strtok(NULL, " /|\n\t\r\\"), NULL, 10);		//parse depth level

			func_name = strtok(NULL, " /|\n\t\r\\");	//parse function name

			//if a function is returning, add num of instructions to the count
			if (strcmp(func_name, "returned.")) {
				count += strtol(strtok(NULL, " /|\n\t\r\\"), NULL, 10);
			}

//			printf("count: %ld\n", count);
	
			//else if ocrDbCreate() is located, record the number of instructions that have
			//passed since the beginning of the program
			if (!strcmp(func_name, "ocrDbCreate()")) {

				//if the array is too small, double its size
				if (blocks->c_count >= blocks->size) {
					blocks->create  = realloc(blocks->create , blocks->size * 2 * sizeof(struct dbc));
					blocks->destroy = realloc(blocks->destroy, blocks->size * 2 * sizeof(struct dbd));
					//initialize newly allocated space
					for (int i = blocks->size; i < blocks->size * 2; i++) {
						dbc_init(blocks->create[i] );
						dbd_init(blocks->destroy[i]);
					} 
					blocks->size *= 2;
				}

				//copy information into data structure and update counter
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
					//initialize newly allocated space
					for (int i = blocks->size; i < blocks->size * 2; i++) {
						dbc_init(blocks->create[i] );
						dbd_init(blocks->destroy[i]);
					} 
					blocks->size *= 2;
				}
				//copy information into data structure and update counter
				blocks->destroy[blocks->d_count].instr_count = count;
				blocks->destroy[blocks->d_count].depth = depth_level;
				blocks->d_count++;
			}
		}
	}
	printf("datablock_parser() - Total instructions counted: %ld.\n", count);


	//This next loop parses the data at the bottom of the file
	//that represents the functions arguments of ocrDb functions
	int dcnt = 0;	
	int ccnt = 0;	
	while(!feof(f)) {
	
		memset ( buf, '\0', BUF_SIZE);		//clear buffer
		fgets(buf, BUF_SIZE, f);
		if (ferror(f)) {
			fprintf(stderr,"Could not read from file %s: %s.\n", filename, strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (feof(f))
			break;

		if ((temp = strtok(buf, " /|\n\t\r\\")) != NULL) {

			//if there is an ocrDbCreate() call, save the arguments
			if (!strcmp(temp, "ocrDbCreate(")) {
				//This loop reads the arguments one by one
				//and converts them to integers instead of strings
				for (int i = 0; i < 6; i++) {
					memset ( buf, '\0', BUF_SIZE);		//clear buffer
					fgets(buf, BUF_SIZE, f);
					if (ferror(f)) {
						fprintf(stderr,"Could not read from file %s: %s.\n", filename, strerror(errno));
						exit(EXIT_FAILURE);
					}
					args[i] = strtol(strtok(buf, " /|\\\n\t\r"), NULL, 16);
				}
				//arguments assigned positions in data structure
				blocks->create[ccnt].id = args[0];
				blocks->create[ccnt].addr = (void *) (args[1]);
				blocks->create[ccnt].len = args[2];
				blocks->create[ccnt].flags = (uint16_t) args[3];
				ccnt++;
			}
			//if there is an ocrDbDestroy() call, save the arguments
			else if (!strcmp(temp, "ocrDbDestroy(")) {
				//save the only argument, the block ID
				fgets(buf, BUF_SIZE, f);
				if (ferror(f)) {
					fprintf(stderr,"Could not read from file %s: %s.\n", filename, strerror(errno));
					exit(EXIT_FAILURE);
				}
				blocks->destroy[dcnt].id = strtol(strtok(buf, " \\|/\n\t\r"), NULL, 0);
				dcnt++;
			}
		}
	}
	if (fclose(f) == -1)
		 fprintf(stderr,"Could not close file pointer to %s: %s.\n", filename, strerror(errno));
}
