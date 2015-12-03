#include "parser.h"
#include "gnuplot.c"

int main(int argc, char** argv)
{
	int create_count=0, destroy_count=0, release_count=0;
	int create_size=1 , destroy_size=1,  release_size=1;
	FILE *f;
	char buf[BUF_SIZE];
	char *token;
	struct DbCreate_info  * DbCreate_data;
	struct DbDestroy_info * DbDestroy_data;
	struct DbRelease_info * DbRelease_data;
	void * tmp = NULL;

	//make sure an input file has been entered
	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	//open input file
	if ((f = fopen(argv[1],"r")) == NULL) {
		fprintf(stderr,"Can't open input file %s\n", argv[1]);
		exit(2);
	}

	//assign memory to each pointer where size=1
	DbCreate_data  = malloc(sizeof(struct DbCreate_info));
	DbDestroy_data = malloc(sizeof(struct DbDestroy_info));
	DbRelease_data = malloc(sizeof(struct DbRelease_info));

	//essentially while(!feof(f)), the break case is below
	//Doing it this way ensures the loop ends exactly when eof is reached
	while (1) {

		//read one line of input file
		fgets(buf, BUF_SIZE, f);
	
		//break case
		if (feof(f))
			break;

		//Extract characters from line until whispace is reached
		token = strtok(buf," \n\t");

		//If this first set of chacters matches "DbCreate:"
		//Then the following information must be recorded in the following manner
		if(token != NULL && !strcmp(token,"DbCreate:")) {

			//if the array is too small for another element, double its size
			if (create_count >= create_size) {
				tmp = (void *)malloc(2 * create_size * sizeof(struct DbCreate_info));
				memcpy(tmp, DbCreate_data, create_size * sizeof(struct DbCreate_info));
				free(DbCreate_data);
				DbCreate_data = (struct DbCreate_info *)tmp;
				tmp = NULL;
				create_size *= 2;
			}

			//Convert each following token into its appropriate data type (instead of string)
			//Save the data in the correct struct variable
			token = strtok(NULL," \n\t");
			DbCreate_data[create_count].time = (clock_t)strtol(token, NULL, 10);

			token = strtok(NULL," \n\t");
			DbCreate_data[create_count].db = (uint64_t)strtol(token, NULL, 0);

			token = strtok(NULL," \n\t");
			DbCreate_data[create_count].addr = (void *)strtol(token, NULL, 0);

			token = strtok(NULL," \n\t");
			DbCreate_data[create_count].len = (uint64_t)strtol(token, NULL, 10);

			token = strtok(NULL," \n\t");
			DbCreate_data[create_count].flags = (uint16_t)strtol(token,NULL,0);

			//increase the position in the array of structs
			create_count++;

			//if there are leftover tokens, there is a possible error
			token = strtok(NULL," \n\t");
			if (token != NULL)
				printf("Warning leftover token: %s\n", token);
		}

		//If this first set of chacters matches "DbDestroy:"
		//Then the following information must be recorded in the following manner
		else if(token != NULL && !strcmp(token,"DbDestroy:")) {

			//if the array is too small for another element, double its size
			if (destroy_count >= destroy_size) {
				tmp = (void *)malloc(2 * destroy_size * sizeof(struct DbDestroy_info));
				memcpy(tmp, DbDestroy_data, destroy_size * sizeof(struct DbDestroy_info));
				free(DbDestroy_data);
				DbDestroy_data = (struct DbDestroy_info *)tmp;
				tmp = NULL;
				destroy_size *= 2;
			}

			//Convert each following token into its appropriate data type (instead of string)
			//Save the data in the correct struct variable
			token = strtok(NULL," \n\t");
			DbDestroy_data[destroy_count].time = (clock_t)strtol(token, NULL, 10);

			token = strtok(NULL," \n\t");
			DbDestroy_data[destroy_count].db = (uint64_t)strtol(token, NULL, 0);

			//increase the position in the array of structs
			destroy_count++;

			//if there are leftover tokens, there is a possible error
			token = strtok(NULL," \n\t");
			if (token != NULL)
				printf("Warning leftover token: %s\n", token);
		}

		//If this first set of chacters matches "DbRelease:"
		//Then the following information must be recorded in the following manner
		else if(token != NULL && !strcmp(token,"DbRelease:")) {

			//if the array is too small for another element, double its size
			if (release_count >= release_size) {
				tmp = (void *)malloc(2 * release_size * sizeof(struct DbRelease_info));
				memcpy(tmp, DbRelease_data, release_size * sizeof(struct DbRelease_info));
				free(DbRelease_data);
				DbRelease_data = (struct DbRelease_info *)tmp;
				tmp = NULL;
				release_size *= 2;
			}

			//Convert each following token into its appropriate data type (instead of string)
			//Save the data in the correct struct variable
			token = strtok(NULL," \n\t");
			DbRelease_data[release_count].time = (clock_t)strtol(token, NULL, 10);

			token = strtok(NULL," \n\t");
			DbRelease_data[release_count].db = (uint64_t)strtol(token, NULL, 0);

			//increase the position in the array of structs
			release_count++;

			//if there are leftover tokens, there is a possible error
			token = strtok(NULL," \n\t");
			if (token != NULL)
				printf("Warning leftover token: %s\n", token);
		}
	}

	//close input file after it had been read
	fclose(f);

	//print each element in the DbCreate array
	printf("\nDB CREATE -----------------------------------------------------------\n");
	for (int i = 0; i < create_count; i++){
		printf("\ttime=%ld db=0x%lx addr=%p len=%lu flags=0x%04x\n",DbCreate_data[i].time,DbCreate_data[i].db,DbCreate_data[i].addr,DbCreate_data[i].len,DbCreate_data[i].flags);
	}
	printf("---------------------------------------------------------------------\n");

	//print each element in the DbDestroy array
	printf("\nDB DESTROY ----------------------------------------------------------\n");
	for (int i = 0; i < destroy_count; i++){
		printf("\ttime=%ld db=0x%lx\n",DbDestroy_data[i].time,DbDestroy_data[i].db);
	}
	printf("---------------------------------------------------------------------\n");

	//print each element in the DbRelease array
	printf("\nDB RELEASE ----------------------------------------------------------\n");
	for (int i = 0; i < release_count; i++){
		printf("\ttime=%ld db=0x%lx\n",DbRelease_data[i].time,DbRelease_data[i].db);
	}
	printf("---------------------------------------------------------------------\n");

	//Print the number of allocations, destructions, and releases 
	printf("\n");
	printf("Number of Datablock creations: %d\n", create_count);
	printf("Number of Datablock destructions: %d\n", destroy_count);
	printf("Number of Datablock releases: %d\n", release_count);
	printf("\n");

//graph data?
//http://stackoverflow.com/questions/3521209/making-c-code-plot-a-graph-automatically

	plot_create_vs_time(DbCreate_data, create_count);
	plot_usage_vs_time(DbCreate_data, DbDestroy_data, create_count+destroy_count);

	return 0;
}
