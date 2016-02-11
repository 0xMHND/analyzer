#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>//for access function

#define NAME_SIZE 32
#define LOG_PATH "\0"
#define SIZE 64

//header
char *get_file_name(int index, char* dir_name, int file_num);
char* create_diff_command(char* name1, char *dir1, char *name2, char* dir2);
void get_dir_name(char* first_dir, char* second_dir);

void example(){
	if (dir == NULL)
		fprintf(stderr, "Cannot open dir %s: %s",dir_name ,strerror(errno));
	exit(1);
}


//create file function
void create_files(){
	int i,j;
	char *file_name = malloc(NAME_SIZE*sizeof(char));
	char *dir_name = malloc(NAME_SIZE*sizeof(char));
	//make a dir
	file_name = "test_dir";
	file_name = "test_file";
	mkdir(dir_name, 0777);
	chdir(dir_name);
	FILE *fp = fopen(file_name, "w");
	//loop:
	fprintf(fp, "hello world");
	//end of loop:
	fclose(fp);
	chdir("..");
//	free(dir_name);
//	free(file_name);
}

//create command for popen function
char* get_dir_name_1(char* first_dir){
	int	flag = 1;
	//names of dir
	first_dir = malloc(32*sizeof(char));
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
	strcpy(command, "diff ");
//	strcat(command, LOG_PATH);
	strcat(command, dir1);
	strcat(command, "/");
	strcat(command, name1);
	strcat(command, " ");
//	strcat(command, LOG_PATH);
	strcat(command, dir2);
	strcat(command, "/");
	strcat(command, name2);
	strcat(command, " > comparing_");
	strcat(command, name1);
	strcat(command, "_and_");
	strcat(command, name2);
	//char buffer[2];
	//snprintf(buffer, 2, "%d", index);
	//strcat(command, buffer);
	strcat(command, ".txt\0");
	return command;
}

char* get_file_name(int index, char* dir_name, int file_num){
 	int i;
	chdir(dir_name);	
	DIR *dp;
	struct dirent *dir;
	char name_array[file_num][SIZE];
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
	char *name = malloc(SIZE*sizeof(char));
	strcpy(name, name_array[index]);
	return name;
}

int get_file_num(char* dir_name){
	int count = 0;
	chdir(dir_name);
	DIR *dp;
	struct dirent *dir;
	dp = opendir(".");
	while ((dir = readdir (dp))!= NULL) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
			count++;	
		}
	}
	closedir(dp);
	chdir("..");
	return count;
}

int main(){
	int i,j,status;
	//fet dir name
	char *dir1;
	char *dir2;
	dir1 = get_dir_name_1(dir1);
	dir2 = get_dir_name_2(dir2);
	//get file numbers
	int file1_num = get_file_num(dir1);
	int file2_num = get_file_num(dir2);
	//get file name
	char file1_names[file1_num][SIZE];
	char file2_names[file2_num][SIZE];
	for (i = 0; i < file1_num; i++)
		strcpy (file1_names[i], get_file_name(i, dir1, file1_num));	
	for (i = 0; i < file2_num; i++)
		strcpy (file2_names[i], get_file_name(i, dir2, file2_num));


	//get command
	char *command = malloc(128*sizeof(char));
	//loop
	for (i = 0; i < file1_num; i++){
		for (j = 0; j < file2_num; j++){
			if (strcmp(file1_names[i],file2_names[j]) == 0){
				strcpy(command, create_diff_command(file1_names[i], dir1, file2_names[j], dir2));
				printf("command is: %s|\n",command);//debug
				status = system(command);	
			}
		}
	}
	free(command);
	free(dir2);
	free(dir1);	
	return 0;
}
