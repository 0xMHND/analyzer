
char * next_folder ()
{
	struct dirent  dir_data;
	DIR * dir;
	char buf[BUF_SIZE];
	char y_n;

	if ((dir = opendir(LOGS_PATH)) == NULL) {
		fprintf(stderr, "Cannot open directory %s: %s.\n", LOGS_PATH, strerror(errno));
		do {
			memset(buf, '\0', BUF_SIZE);
			printf("Make new %s folder? (y/n)   > ", LOGS_PATH);
			scanf(" %s",&buf);
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

	return buf;
}
