#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/***************************
 * input: 	none
 * output:	A string containing a path to the folder to save data to
 * description:	This function looks into the logs directory and creates a new one if needed.
 * 				Then, it determines what the folder name should be and returns the path to save to it.
***************************/
char * next_folder ()
{
	struct dirent * dir_data;
	DIR * dir;
	struct stat s;
	int run = 0, temp;
	char * buf = malloc(BUF_SIZE * sizeof(char));
	char y_n, num[RUN_NUM_DIGITS + 1];

	//Open the path where the logs are stored
	if ((dir = opendir(LOGS_PATH)) == NULL) {
		fprintf(stderr, "Cannot open directory %s: %s.\n", LOGS_PATH, strerror(errno));

		//prompt the user if he wants to create a new logs folder if none is present
		//loop until correct input is provided (y/n)
		do {
			memset(buf, '\0', BUF_SIZE);
			printf("Make new %s folder? (y/n) > ", LOGS_PATH);
			scanf(" %s",buf);
			y_n = buf[0];

			//if yes, create the new folder and open the directory
			if (y_n == 'y') {
				printf("Creating %s directory...\n", LOGS_PATH);
				if (mkdir(LOGS_PATH, 0777) == -1) {
					fprintf(stderr, "Could not create %s directory: %s.\n", LOGS_PATH, strerror(errno));
					return NULL;
				}
				if ((dir = opendir(LOGS_PATH)) == NULL) {
					fprintf(stderr, "Cannot open directory %s: %s.\n", LOGS_PATH, strerror(errno));
					return NULL;
				}
			}
			//if no, exit
			else if (y_n == 'n') {
				printf("Exiting.\n");
				exit(1);
			}
		} while (y_n != 'y' && y_n != 'n');
	}

	//Go through each file and figure out what the name of the folder for the next results will be.
	//For example, if there is a folder named run001, run002, run003, then the next one will be run004
	while ((dir_data = readdir(dir)) != NULL) {
		memset(buf, '\0', BUF_SIZE);
		
		if (snprintf(buf, BUF_SIZE, "%s%s", LOGS_PATH, dir_data->d_name) > BUF_SIZE) {
			fprintf(stderr, "Warning Buffer overflow %s: %s\n", buf, strerror(errno));
		}		

		if (stat(buf, &s) == -1) {
			fprintf(stderr, "Could not stat file %s: %s.\n", buf, strerror(errno));
			return NULL;
		}
		if (S_ISDIR(s.st_mode) && dir_data->d_name[0] != '.') {
			if (dir_data->d_name[0] == 'r' && dir_data->d_name[1] == 'u' && dir_data->d_name[2] == 'n') {
				strcpy(num, dir_data->d_name + 3);
				temp = strtol(num, NULL, 10);
				if (temp > run)
					run = temp;
			}
		}
	}
	closedir(dir);
	run++;

	memset(buf, '\0', BUF_SIZE);
	if (snprintf(buf, BUF_SIZE, "%srun%03d%c", LOGS_PATH, run, '/') > BUF_SIZE) {
		fprintf(stderr, "Buffer overflow %s: %s.\n", buf, strerror(errno));
	}
		
	if (mkdir(buf, 0777) == -1) {
		fprintf(stderr, "Could not create %s directory: %s.\n", buf, strerror(errno));
		return NULL;
	}

	return buf;
}
