#include <stdio.h>
#include <stdlib.h>

#define READ 0
#define WRITE 1

int formatter_write( void * ptr, int type, char * path)
{
	char buf[BUF_SIZE], full_path[BUF_SIZE];
	FILE * f;
	//type 1: ocr_funcions
	if (type == OCR_FUNCTIONS) {
		//cast to block_info
		struct block_info * blocks = (struct block_info *) ptr;

		if (snprintf(full_path, BUF_SIZE, "%s%s", path, "ocrDb_functions.out") >= BUF_SIZE)
			printf("Warning: Buffer size exceeded in formatter_write()\n");
		//check readibility of files
		if ((f = fopen(full_path, "w")) == NULL) {
			fprintf(stderr, "Could not open file %s: %s.\n", full_path, strerror(errno));
			return 1;
		}

		//Write the magic string at the top of the file
		if (fputs(OCR_FUNCTIONS_MAGIC, f) == EOF)
			fprintf(stderr, "Could not put \"%s\" in %s: %s.\n", OCR_FUNCTIONS_MAGIC, full_path, strerror(errno));

		//write the creation count
		if (snprintf(buf, BUF_SIZE, "c_count %d\n", blocks->c_count) >= BUF_SIZE)
			printf("Warning: Buffer size exceeded in formatter_write()\n");
		if (fputs(buf, f) == EOF)
			fprintf(stderr, "Could not put \"%s\" in %s: %s.\n", buf, full_path, strerror(errno));
	
		//write the destruction count
		if (snprintf(buf, BUF_SIZE, "d_count %d\n", blocks->d_count) >= BUF_SIZE)
			printf("Warning: Buffer size exceeded in formatter_write()\n");
		if (fputs(buf, f) == EOF)
			fprintf(stderr, "Could not put \"%s\" in %s: %s.\n", buf, full_path, strerror(errno));
		
		if (fclose(f) == EOF)
			fprintf(stderr, "Could not close file pointer to %s: %s.\n", full_path, strerror(errno));

		//write the array of creation times
		for (int i = 0; i < blocks->c_count; i++) {
			if (snprintf(buf, BUF_SIZE, "create %ld %ld %ld %p %ld %d\n", blocks->create[i].instr_count, blocks->create[i].depth, blocks->create[i].dbid, blocks->create[i].addr, blocks->create[i].len, blocks->create[i].flags) >= BUF_SIZE)
				printf("Warning: Buffer size exceeded in formatter_write()\n");
			if (fputs(buf, f) == EOF)
				fprintf(stderr, "Could not put \"%s\" in %s: %s.\n", buf, full_path, strerror(errno));
		}

		//write the array of destruction times
		for (int i = 0; i < blocks->d_count; i++) {
			if (snprintf(buf, BUF_SIZE, "destroy %ld %ld %ld\n", blocks->destroy[i].instr_count, blocks->destroy[i].depth, blocks->destroy[i].dbid) >= BUF_SIZE)
				printf("Warning: Buffer size exceeded in formatter_write()\n");
			if (fputs(buf, f) == EOF)
				fprintf(stderr, "Could not put \"%s\" in %s: %s.\n", buf, full_path, strerror(errno));
		}
	}

	//else, if it's processed data, ignore for now
	else if (type == PROCESSED_DATA) {
		snprintf(full_path, BUF_SIZE, "%s%s", path, "processed_data.out");
		if ((f = fopen(full_path, "w")) == NULL) {
			fprintf(stderr, "Could not open file %s: %s.\n", full_path, strerror(errno));
			return 1;
		}

		if (fclose(f) == EOF)
			fprintf(stderr, "Could not close file pointer to %s: %s.\n", full_path, strerror(errno));
	}

	//else, if it's a memory analysis, ignore for now
	else if (type == MEMORY_ANALYSIS) {
		snprintf(full_path, BUF_SIZE, "%s%s", path, "memory_analysis.out");
		if ((f = fopen(full_path, "w")) == NULL) {
			fprintf(stderr, "Could not open file %s: %s.\n", full_path, strerror(errno));
			return 1;
		}

		if (fclose(f) == EOF)
			fprintf(stderr, "Could not close file pointer to %s: %s.\n", full_path, strerror(errno));
	}

	return 0;
}




// FOR FUTURE USE MAYBE???

/*
void * formatter_read( int run, int type, char * path)
{
	void * result;


	return result;
}
*/
