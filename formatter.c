#include <stdio.h>
#include <stdlib.h>

#define READ 0
#define WRITE 1

/***************************
 * input: 	Pointer that contains all information to be written to a file
 * 			An integer representing the type of the previously mentioned pointer
 * 			A string containing the path to save data to
 * output:	This function returns 0 on success
 * description:	This function takes in different types of struct pointers in the first argument,
 * 				and then extracts all information from those structs and writes that to a file.
***************************/
int formatter_write( void * ptr, int type)
{
	char buf[BUF_SIZE], full_path[BUF_SIZE];
	FILE * f;
	//type 1: ocr_funcions
	if (type == OCR_FUNCTIONS) {
		//cast to block_info
		struct block_info * blocks = (struct block_info *) ptr;

		if (snprintf(full_path, BUF_SIZE, "%s%s", LOG_PATH, "ocrDb_functions.out") >= BUF_SIZE)
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

		//write the array of creation times
		for (int i = 0; i < blocks->c_count; i++) {
			if (snprintf(buf, BUF_SIZE, "create %ld %#lx %p %ld %#04x\n", blocks->create[i].instr_count, blocks->create[i].id, blocks->create[i].addr, blocks->create[i].len, blocks->create[i].flags) >= BUF_SIZE)
				printf("Warning: Buffer size exceeded in formatter_write()\n");
			if (fputs(buf, f) == EOF)
				fprintf(stderr, "Could not put \"%s\" in %s: %s.\n", buf, full_path, strerror(errno));
		}

		//write the array of destruction times
		for (int i = 0; i < blocks->d_count; i++) {
			if (snprintf(buf, BUF_SIZE, "destroy %ld %#lx\n", blocks->destroy[i].instr_count, blocks->destroy[i].id) >= BUF_SIZE)
				printf("Warning: Buffer size exceeded in formatter_write()\n");
			if (fputs(buf, f) == EOF)
				fprintf(stderr, "Could not put \"%s\" in %s: %s.\n", buf, full_path, strerror(errno));
		}		

		if (fclose(f) == EOF)
			fprintf(stderr, "Could not close file pointer to %s: %s.\n", full_path, strerror(errno));
	}

	//else, if it's processed data, ignore for now
	else if (type == PROCESSED_DATA) {
		int status;
		struct lifetime_master * master = (struct lifetime_master *)ptr;

		snprintf(full_path, BUF_SIZE, "%s%s", LOG_PATH, "processed_data.out");
		if ((f = fopen(full_path, "w")) == NULL) {
			fprintf(stderr, "Could not open file %s: %s.\n", full_path, strerror(errno));
			return 1;
		}

		status = fprintf(f,	"%s", PROCESSED_DATA_MAGIC);
		if (status < 0)
			fprintf(stderr, "Could not write to file %s: %s.\n", full_path, strerror(errno));

		for(int i=0; i<master->size;i++){
			long unsigned int address = (long unsigned int)master->ptr[i]->addr;
			status = fprintf(f,	"Block #%d,\n\tID #%#lx\n", i, master->ptr[i]->id);
			if (status < 0)
				fprintf(stderr, "Could not write to file %s: %s.\n", full_path, strerror(errno));
			status = fprintf(f, 	"\tAddr. #%#lx\n", address);
			if (status < 0)
				fprintf(stderr, "Could not write to file %s: %s.\n", full_path, strerror(errno));
			status = fprintf(f,	"\tSize %ld Bytes\n", (long int) master->ptr[i]->len);
			if (status < 0)
				fprintf(stderr, "Could not write to file %s: %s.\n", full_path, strerror(errno));
			status = fprintf(f,	"\tFlags #%#x\n",  master->ptr[i]->flags);
			if (status < 0)
				fprintf(stderr, "Could not write to file %s: %s.\n", full_path, strerror(errno));
			status = fprintf(f, 	"\tStart Time: %lf\n", (double) master->ptr[i]->instr_count);
			if (status < 0)
				fprintf(stderr, "Could not write to file %s: %s.\n", full_path, strerror(errno));
			status = fprintf(f,	"\tEnd Time:   %lf\n", (double) master->ptr[i]->partner->instr_count);
			if (status < 0)
				fprintf(stderr, "Could not write to file %s: %s.\n", full_path, strerror(errno));
			status = fprintf(f,	"\tlifetime: %lf\n", (double) master->ptr[i]->lifetime_instructions);
			if (status < 0)
				fprintf(stderr, "Could not write to file %s: %s.\n", full_path, strerror(errno));
			status = fprintf(f,	"\tinstr. %lf" "seconds\n", (double)master->ptr[i]->lifetime_time);
			if (status < 0)
				fprintf(stderr, "Could not write to file %s: %s.\n", full_path, strerror(errno));
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
