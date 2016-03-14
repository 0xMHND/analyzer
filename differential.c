#include "main.h"

volatile int VERBOSE = 0;

/***************************
 * input: index of which file, dir name, number of files
 * output: one file name
 * description:	get file names from a dir
***************************/
char* get_file_name(int index, char* dir_name, int file_num);

/***************************
 * input: input from keyboard
 * output: one dir name
 * description:	get dir name from user
***************************/
char* get_dir_name(int num);


/***************************
 * input: name of a dir
 * output: number of files in the dir
 * description:	count the number of files in a dir
***************************/
int get_file_num(char* dir_name);


/***************************
 * input: name of 2 dirs
 * output: name of the file out logs in
 * description:	it combine 2 dir names and generate a new dir name
***************************/
char *get_save_dir_name(char *dir1, char *dir2);

char *get_save_dir_name(char *dir1, char *dir2) {
	//length of my dir name
	char *name = malloc(BUF_SIZE * sizeof(char));
	//format is compare.logs.dir1_and_dir2
	sprintf(name, "diff-u_%s_%s", dir1, dir2);
	return name;
}

char* get_dir_name(int num){
	int	flag = 1;
	//names of dir
	char * dir = malloc(BUF_SIZE*sizeof(char));
	char buf[BUF_SIZE];
	//to pass in get_name function 
	//input 1
	while (flag == 1){
	printf("Enter directory name %d >", num);
	scanf("%s", dir);
	//input 1 error handle
	
	if (snprintf(buf, BUF_SIZE, "%s%s", LOGS_PATH, dir) < BUF_SIZE){
		fprintf(stderr, "snprintf: %s",strerror(errno));
	}
	if (access(buf, F_OK)!= -1)
		flag = 0;
	else
		printf("Invalid dir name.\n");
	}
	return dir;
}

char* get_file_name(int index, char* dir_name, int file_num){
 	int i;
	DIR *dp;	
	struct dirent *dir;
	//2d array for the names of files
	char name_array[file_num][BUF_SIZE];
	//error check for open a dir
	char buf[BUF_SIZE];

	if (snprintf(buf, BUF_SIZE, "%s%s", LOGS_PATH, dir_name) < BUF_SIZE){
		fprintf(stderr, "snprintf: %s",strerror(errno));
	}

	dp = opendir (buf);
	i = 0;
	while ((dir = readdir (dp))!= NULL) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
				strcpy (name_array[i], dir->d_name);
				i++;
		}
	}
	closedir(dp);
	//return the ith of the file
	char *name = malloc(BUF_SIZE*sizeof(char));
	strcpy(name, name_array[index]);
	return name;
}

int get_file_num(char* dir_name){
	int count = 0;
	DIR *dp;
	struct dirent *dir;
	char buf[BUF_SIZE];

	if (snprintf(buf, BUF_SIZE, "%s%s", LOGS_PATH, dir_name) < BUF_SIZE){
		fprintf(stderr, "snprintf: %s",strerror(errno));
	}

	dp = opendir(buf);
	//count the files
	while ((dir = readdir (dp))!= NULL) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
			count++;	
		}
	}
	closedir(dp);
	return count;
}

int main(int argc, char ** argv){

	char buffer[BUF_SIZE];
	int i,j,status;
	char *dir1=NULL;
	char *dir2=NULL;
	char command[BUF_SIZE];

	//error checking input
	if (argc > 2) {
		printf("Usage: %s [-v]\n", argv[0]);
		exit(1);
	}
	// -v represent for verbose setting
	else if (argc == 2) {
		if (!strcmp(argv[1],"-v"))
			VERBOSE = 1;
		else {
			printf("Invalid Argument. Usage %s [-v]\n", argv[0]);
			return 1;
		}
	}

	//get fir names
	dir1 = get_dir_name(1);
	dir2 = get_dir_name(2);
	//open files
	char *save_dir_name = get_save_dir_name(dir1, dir2);
	if (snprintf(buffer, BUF_SIZE, "%s%s", LOGS_PATH, save_dir_name) < BUF_SIZE){
		fprintf(stderr, "snprintf: %s",strerror(errno));
	}
	if (mkdir(buffer, 0777)){
	//handle stuff
	}

	if (VERBOSE == 1){
		printf("The first dir been chosen is:  %s\n", dir1);
		printf("The second dir been chosen is: %s\n", dir2);
	}

	int file1_num = get_file_num(dir1);
	int file2_num = get_file_num(dir2);
	if (VERBOSE == 1){
		printf("%d files in %s.\n", file1_num, dir1);
		printf("%d files in %s.\n", file2_num, dir2);
	}

	//get file name
	char file1_names[file1_num][BUF_SIZE];
	char file2_names[file2_num][BUF_SIZE];

	for (i = 0; i < file1_num; i++)
		strcpy (file1_names[i], get_file_name(i, dir1, file1_num));	
	for (i = 0; i < file2_num; i++)
		strcpy (file2_names[i], get_file_name(i, dir2, file2_num));

	//loop to find the same names of files and compare them, 2 files and a time
	for (i = 0; i < file1_num; i++){
		for (j = 0; j < file2_num; j++){
			if (strcmp(file1_names[i],file2_names[j]) == 0){

				if (snprintf(command, BUF_SIZE, "diff -u %s%s/%s %s%s/%s >> %s%s/%s.out", LOGS_PATH, dir1, file1_names[i], LOGS_PATH, dir2, file2_names[j], LOGS_PATH, save_dir_name, file1_names[i]) < BUF_SIZE){
					fprintf(stderr, "snprintf: %s",strerror(errno));
				}

				printf("dfgsnlkfds: %s\n", command);
				printf("Comparing: %s for %s and %s. Continue? (yes/no) ", file1_names[i], dir1, dir2);
				scanf("%s", buffer);
				if(!strcmp(buffer, "yes")){
					status = system(command);
					if (status == -1){
						fprintf(stderr, "Cannot run this command %s: %s\n",command ,strerror(errno));
						exit(1);
					}
				}				
			}
		}
	}
	free(dir2);
	free(dir1);
	free(save_dir_name);
	if (VERBOSE == 1)
		printf("program done, exit 0.\n");	
	return 0;
}
