#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void datablock_parser(struct block_info * blocks, char * filename)
{
	int instr_count = 0;
	FILE * f;
	char buf[BUF_SIZE];
	char * func_name;

	if ( (f = fopen( filename, "r")) == NULL ) {
		perror("Cannot open file specified");
		exit(1);
	}

	while(!feof(f)) {

		memset ( buf, '\0', BUF_SIZE);
		fgets( buf, BUF_SIZE, f);

		if (strtok( buf, " /|\n\t\r\\") == NULL)
			break;

		//PC
		strtok(NULL, " /|\n\t\r\\");

		//depth level
		strtok(NULL, " /|\n\t\r\\");

		func_name = strtok(NULL, " /|\n\t\r\\");

//		if ((temp = strtok(NULL, " /|\n\t\r\\")) != NULL) {
//			instr_count += atoi(temp);
//		}
//		printf("level: %d\n", level);

		if (!strcmp(func_name, "returned.")) {
			instr_count += atoi(strtok(NULL, " /|\n\t\r\\"));
		}	
		else if (!strcmp(func_name, "ocrDbCreate()")) {

			if (blocks->c_count >= blocks->size) {
				blocks->size *= 2;
				blocks->create  = realloc(blocks->create , blocks->size*sizeof(long));
				blocks->destroy = realloc(blocks->destroy, blocks->size*sizeof(long));
			}
			blocks->create[blocks->c_count] = instr_count;
			blocks->c_count++;
		}
		else if (!strcmp(func_name, "ocrDbDestroy()")) {
			if (blocks->d_count >= blocks->size) {
				blocks->size *= 2;
				blocks->create  = realloc(blocks->create , blocks->size*sizeof(long));
				blocks->destroy = realloc(blocks->destroy, blocks->size*sizeof(long));
			}
			blocks->destroy[blocks->d_count] = instr_count;
			blocks->d_count++;
		}
	}

}
