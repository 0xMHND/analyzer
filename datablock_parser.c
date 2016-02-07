#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 256
#define DEF_SIZE 16

int main(int argc, char ** argv)
{
	long * create, * destroy;
	int level, instr_count = 0, size = DEF_SIZE, c_count = 0, d_count = 0;
	FILE * f;
	char buf[BUF_SIZE];
	char * func_name;

	if ( argc != 2 ) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	if ( (f = fopen( argv[1], "r")) == NULL ) {
		perror("Cannot open file specified");
		exit(1);
	}

	create  = malloc(size*sizeof(long));
	destroy = malloc(size*sizeof(long));

	while(!feof(f)) {

		fgets( buf, BUF_SIZE, f);

		if (strtok( buf, " /|\n\t\r\\") == NULL)
			break;

		strtok(NULL, " /|\n\t\r\\");
		level = atoi(strtok(NULL, " /|\n\t\r\\"));
		func_name = strtok(NULL, " /|\n\t\r\\");

//		if ((temp = strtok(NULL, " /|\n\t\r\\")) != NULL) {
//			instr_count += atoi(temp);
//		}
		printf("level: %d\n", level);

		if (!strcmp(func_name, "returned.")) {
			instr_count += atoi(strtok(NULL, " /|\n\t\r\\"));
		}	
		else if (!strcmp(func_name, "ocrDbCreate()")) {

			if (c_count >= size) {
				size *= 2;
				create  = realloc(create , size*sizeof(long));
				destroy = realloc(destroy, size*sizeof(long));
			}
			create[c_count] = instr_count;
			c_count++;
		}
		else if (!strcmp(func_name, "ocrDbDestroy()")) {
			if (d_count >= size) {
				size *= 2;
				create  = realloc(create , size*sizeof(long));
				destroy = realloc(destroy, size*sizeof(long));
			}
			destroy[d_count] = instr_count;
			d_count++;
		}
	}

	for (int i = 0; i < c_count; i++)
		printf("%ld  ",create[i]);
	printf("\n");
	for (int i = 0; i < d_count; i++)
		printf("%ld  ",destroy[i]);
	printf("\n");

	return 0;
}
