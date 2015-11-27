#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define BUF_SIZE 256
//#define format string

struct DbCreate_info {
	void *db;
	void *addr;
	uint64_t len;
	uint16_t flags;
};

struct DbDestroy_info {
	void *db;
};

struct DbRelease_info {
	void *db;
};

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

	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	if ((f = fopen(argv[1],"r")) == NULL) {
		fprintf(stderr,"Can't open input file %s\n", argv[1]);
		exit(2);
	}

	DbCreate_data  = malloc(sizeof(struct DbCreate_info));
	DbDestroy_data = malloc(sizeof(struct DbDestroy_info));
	DbRelease_data = malloc(sizeof(struct DbRelease_info));

	while (1) {

		fgets(buf, BUF_SIZE, f);
	
		if (feof(f))
			break;

		token = strtok(buf," \n\t");

		if(token != NULL && !strcmp(token,"DbCreate:")) {

			if (create_count >= create_size) {
				tmp = (void *)malloc(2 * create_size * sizeof(struct DbCreate_info));
				memcpy(tmp, DbCreate_data, create_size * sizeof(struct DbCreate_info));
				free(DbCreate_data);
				DbCreate_data = (struct DbCreate_info *)tmp;
				tmp = NULL;
				create_size *= 2;
			}

			token = strtok(NULL," \n\t");
			DbCreate_data[create_count].db = (void *)strtol(token, NULL, 0);

			token = strtok(NULL," \n\t");
			DbCreate_data[create_count].addr = (void *)strtol(token, NULL, 0);

			token = strtok(NULL," \n\t");
			DbCreate_data[create_count].len = (uint64_t)strtol(token, NULL, 10);

			token = strtok(NULL," \n\t");
			DbCreate_data[create_count].flags = (uint16_t)strtol(token,NULL,0);

			create_count++;

			token = strtok(NULL," \n\t");
			if (token != NULL)
				printf("Warning leftover token: %s\n", token);
		}

		else if(token != NULL && !strcmp(token,"DbDestroy:")) {

			if (destroy_count >= destroy_size) {
				tmp = (void *)malloc(2 * destroy_size * sizeof(struct DbDestroy_info));
				memcpy(tmp, DbDestroy_data, destroy_size * sizeof(struct DbDestroy_info));
				free(DbDestroy_data);
				DbDestroy_data = (struct DbDestroy_info *)tmp;
				tmp = NULL;
				destroy_size *= 2;
			}

			token = strtok(NULL," \n\t");
			DbDestroy_data[destroy_count].db = (void *)strtol(token, NULL, 0);

			destroy_count++;

			token = strtok(NULL," \n\t");
			if (token != NULL)
				printf("Warning leftover token: %s\n", token);
		}

		else if(token != NULL && !strcmp(token,"DbRelease:")) {

			if (release_count >= release_size) {
				tmp = (void *)malloc(2 * release_size * sizeof(struct DbRelease_info));
				memcpy(tmp, DbRelease_data, release_size * sizeof(struct DbRelease_info));
				free(DbRelease_data);
				DbRelease_data = (struct DbRelease_info *)tmp;
				tmp = NULL;
				release_size *= 2;
			}

			token = strtok(NULL," \n\t");
			DbRelease_data[release_count].db = (void *)strtol(token, NULL, 0);

			release_count++;

			token = strtok(NULL," \n\t");
			if (token != NULL)
				printf("Warning leftover token: %s\n", token);
		}
	}

	fclose(f);

	printf("\nDB CREATE -----------------------------------------------------------\n");
	for (int i = 0; i < create_count; i++){
		printf("\tdb=%p addr=%p len=%lu flags=0x%04x\n",DbCreate_data[i].db,DbCreate_data[i].addr,DbCreate_data[i].len,DbCreate_data[i].flags);
	}
	printf("---------------------------------------------------------------------\n");

	printf("\nDB DESTROY ----------------------------------------------------------\n");
	for (int i = 0; i < destroy_count; i++){
		printf("\tdb=%p\n",DbDestroy_data[i].db);
	}
	printf("---------------------------------------------------------------------\n");

	printf("\nDB RELEASE ----------------------------------------------------------\n");
	for (int i = 0; i < release_count; i++){
		printf("\tdb=%p\n",DbRelease_data[i].db);
	}
	printf("---------------------------------------------------------------------\n");

	printf("\n");
	printf("Number of Datablock creations: %d\n", create_count);

	printf("Number of Datablock destructions: %d\n", destroy_count);

	printf("Number of Datablock releases: %d\n", release_count);
	printf("\n");

	return 0;
}
