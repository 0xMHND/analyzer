#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char * next_folder ()
{
	struct dirent * dir_data;
	DIR * dir;
	struct stat s;
	int run = 0, temp;
	char * buf = malloc(BUF_SIZE * sizeof(char));
	char y_n, num[RUN_NUM_DIGITS + 1];

	if ((dir = opendir(LOGS_PATH)) == NULL) {
		fprintf(stderr, "Cannot open directory %s: %s.\n", LOGS_PATH, strerror(errno));
		do {
			memset(buf, '\0', BUF_SIZE);
			printf("Make new %s folder? (y/n) > ", LOGS_PATH);
			scanf(" %s",buf);
			y_n = buf[0];

			if (y_n == 'y') {
				memset(buf, '\0', BUF_SIZE);
				snprintf(buf, BUF_SIZE, "mkdir %s", LOGS_PATH);
				system(buf);
			}
			else if (y_n == 'n') {
				printf("Exiting.\n");
				exit(1);
			}
		} while (y_n != 'y' && y_n != 'n');
	}

	while ((dir_data = readdir(dir)) != NULL) {
		memset(buf, '\0', BUF_SIZE);
		snprintf(buf, BUF_SIZE, "%s%s", LOGS_PATH, dir_data->d_name);

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
	run++;

	memset(buf, '\0', BUF_SIZE);
	snprintf(buf, BUF_SIZE, "mkdir %srun%03d", LOGS_PATH, run);
	printf("buf:%s\n", buf);
	if (system(buf) == -1) {
		fprintf(stderr, "Could not make new directory to store logs (\"%s\"): %s.\n", buf, strerror(errno));
		return NULL;
	}

	memset(buf, '\0', BUF_SIZE);
	snprintf(buf, BUF_SIZE, "%srun%03d%c", LOGS_PATH, run, '/');
	printf("buf:%s\n", buf);
	return buf;
}
