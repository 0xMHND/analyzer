#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>//for access function
#include <errno.h>
#include <sys/types.h>


#define NAME_SIZE 32
#define LOGS_PATH "../"
#define SIZE 64

volatile int VERBOSE = 0;

/***************************
 * input: index of which file, dir name, number of files
 * output: one file name
 * description:	get file names from a dir
***************************/
char *get_file_name(int index, char* dir_name, int file_num);


/***************************
 * input: input from keyboard
 * output: one dir name
 * description:	get dir name from user
***************************/
char* get_dir_name_1(char* first_dir);


/***************************
 * input: 2 names of files and 2 names of dirs
 * output: command fed in differ file
 * description:	generate command instruction
***************************/
char* create_diff_command(char* name1, char *dir1, char *name2, char* dir2);


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

//example error output
/*
void example(){
	if (dir == NULL)
		fprintf(stderr, "Cannot open dir %s: %s",dir_name ,strerror(errno));
	exit(1);
}
*/
char *get_save_dir_name(char *dir1, char *dir2) {
	//length of my dir name
	char *name = malloc(SIZE * sizeof(char));
	//format is compare.logs.dir1_and_dir2
	sprintf(name, "compare.logs.%s_and_%s", dir1, dir2);
	return name;
}

char* get_dir_name_1(char* first_dir){
	int	flag = 1;
	//names of dir
	first_dir = malloc(SIZE*sizeof(char));
	//to pass in get_name function 
	//input 1
	while (flag == 1){
	printf("First dir name? >");
	scanf("%s", first_dir);
	//input 1 error handle
	if (access(first_dir, F_OK)!= -1)
		flag = 0;
	else
		printf("Invalid dir name.\n");
	}
	return first_dir;
}
char* get_dir_name_2(char* second_dir){
	int flag = 1;
	second_dir = malloc(32*sizeof(char));
	while (flag == 1){
	printf("Second dir name? >");
	scanf("%s", second_dir);
	//input 2 error handle
	if( access( second_dir, F_OK ) != -1 ) 
		flag = 0;
	else 
 		printf("Invalid dir name.\n");
	}
	return second_dir;	
}

char* create_diff_command(char* name1, char *dir1, char *name2, char* dir2){

	//create the command to fed in system	
	char* command = malloc(128*sizeof(char));
	strcpy(command, "diff -u ");
	strcat(command, LOGS_PATH);
	strcat(command, dir1);
	strcat(command, "/");
	strcat(command, name1);
	strcat(command, " ");
	strcat(command, LOGS_PATH);
	strcat(command, dir2);
	strcat(command, "/");
	strcat(command, name2);
	printf("The default file name is: comparing %s_and_%s\n", name1, name2);
	char input[40];
	printf("Input yes to continue, input new file name to change.\n");
	scanf("%s", input);
	//if input is yes, use the default name
	if (!strcmp(input, "yes")){
		strcat(command, " > comparing_");
		strcat(command, name1);
		strcat(command, "_and_");
		strcat(command, name2);
		strcat(command, ".txt\0");
	}
	//else use the input as name
	else {
		strcat(command, " > ");
		strcat(command, input);
		strcat(command, "\0");
	}
	if (VERBOSE == 1)
		printf ("Program running:\n%s\n", command);
	return command;
}

char* get_file_name(int index, char* dir_name, int file_num){
 	int i;
	DIR *dp;	
	struct dirent *dir;
	//2d array for the names of files
	char name_array[file_num][SIZE];
	//error check for open a dir
	if (chdir(dir_name)== -1){
		fprintf(stderr, "Cannot change to dir %s: %s\n",dir_name ,strerror(errno));
		printf("1\n");
		exit(1);
	}
	dp = opendir (".");
	i = 0;
	while ((dir = readdir (dp))!= NULL) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
				strcpy (name_array[i], dir->d_name);
				i++;
			}
		}
	closedir(dp);
	chdir("..");
	//return the ith of the file
	char *name = malloc(SIZE*sizeof(char));
	strcpy(name, name_array[index]);
	return name;
}

int get_file_num(char* dir_name){
	int count = 0;
	if (chdir(dir_name)== -1){
		fprintf(stderr, "Cannot change to dir %s: %s\n",dir_name ,strerror(errno));
		printf("2\n");
		exit(1);
	}	
	DIR *dp;
	struct dirent *dir;
	dp = opendir(".");
	//count the files
	while ((dir = readdir (dp))!= NULL) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
			count++;	
		}
	}
	closedir(dp);
	chdir("..");
	return count;
}

int main(int argc, char ** argv){

	char buffer[10];
	int i,j,status;
	//fet dir name
	char *dir1=NULL;
	char *dir2=NULL;
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
	dir1 = get_dir_name_1(dir1);
	dir2 = get_dir_name_2(dir2);
	//open files
	char *save_dir_name = get_save_dir_name(dir1, dir2);
	mkdir(save_dir_name, 0777);
	if (VERBOSE == 1){
		printf("The first dir been chosen is:  %s\n", dir1);
		printf("The second dir been chosen is: %s\n", dir2);
	}
	//get file numbers
	int file1_num = get_file_num(dir1);
	int file2_num = get_file_num(dir2);
	if (VERBOSE == 1){
		printf("%d files in dir1.\n", file1_num);
		printf("%d diles in dir2.\n", file2_num);
	}
	//get file name
	char file1_names[file1_num][SIZE];
	char file2_names[file2_num][SIZE];
	for (i = 0; i < file1_num; i++)
		strcpy (file1_names[i], get_file_name(i, dir1, file1_num));	
	for (i = 0; i < file2_num; i++)
		strcpy (file2_names[i], get_file_name(i, dir2, file2_num));
	//get command
	char *command = malloc(128*sizeof(char));
	//loop to find the same names of files and compare them, 2 files and a time
	for (i = 0; i < file1_num; i++){
		for (j = 0; j < file2_num; j++){
			if (strcmp(file1_names[i],file2_names[j]) == 0){
				strcpy(command, create_diff_command(file1_names[i], dir1, file2_names[j], dir2));
				
				printf("The file comparing are %s and %s.\n", file1_names[i], file2_names[j]);
				printf("Enter yes to compare, input no to cancel the compare.\n");
				scanf("%s", buffer);
				if(!strcmp(buffer, "yes")){
					chdir(save_dir_name);
					status = system(command);
					chdir("..");
					if (status == -1){
						fprintf(stderr, "Cannot run this command %s: %s\n",command ,strerror(errno));
						exit(1);
					}
				}				
			}
		}
	}
	free(command);
	free(dir2);
	free(dir1);
	free(save_dir_name);
	if (VERBOSE == 1)
		printf("program done, exit 0.\n");	
	return 0;
}
