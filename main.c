#include <stdio.h>
#include <stdlib.h>
#include <regex.h>        
#include <string.h>

#define BUF_SIZE 256

struct arg
{
	char * type;
	long val;
};

struct step
{
	long pc;
	char *instr;
	struct arg ** inputs;
	struct arg ** outputs;
	int in_size;
	int out_size;
};

char * match2string (regmatch_t match, char * buf)
{
	char *dest = malloc((match.rm_eo - match.rm_so + 1) * sizeof(char));
	strncpy(dest, buf + match.rm_so, match.rm_eo - match.rm_so);
	dest[match.rm_eo - match.rm_so] = '\0';
	return dest;
}

int main(int argc, char ** argv)
{
	struct step ** steps;
	char buf[BUF_SIZE];
	char * temp, token;
	regex_t core_regex;
	int core_reti;
	FILE *f;

	//printf("\\[TG_TRACE_XE_INSN\\] \\[PC (0x[0-9a-f]*)\\] Executed '([0-9a-zA-Z]+)': (Output: [0-9a-z_=, ]*)?(Input: [0-9a-z_=, ]*)?");
	core_reti = regcomp(&core_regex, "\\[TG_TRACE_XE_INSN\\] \\[PC (0x[0-9a-f]*)\\] Executed '([0-9a-zA-Z]+)': (Output: [0-9a-z_=, ]*)?(Input: [0-9a-z_=, ]*)?", REG_EXTENDED);
	if (core_reti) {
		fprintf(stderr, "Could not compile regex\n");
		exit(1);
	}

	size_t core_ngroups = core_regex.re_nsub + 1;
	regmatch_t *core_groups = malloc(core_ngroups * sizeof(regmatch_t));

	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}


	//open input file
	if ((f = fopen(argv[1],"r")) == NULL) {
		fprintf(stderr,"Can't open input file %s\n", argv[1]);
		exit(2);
	}

	//essentially while(!feof(f)), the break case is below
	//Doing it this way ensures the loop ends exactly when eof is reached
	int i = 0;
	int j=0;
	while (1) {
		//read one line of input file
		for (j = 0; j < BUF_SIZE; j++)
			buf[j] = '\0';
		fgets(buf, BUF_SIZE, f);

		//break case
		if (feof(f))
			break;


		/* Execute regular expression */
		core_reti = regexec(&core_regex, buf, core_ngroups, core_groups, 0);
		if (!core_reti) {
//			printf("SUCCESS1: %s | %s | %s | %s\n", match2string(core_groups[1],buf), match2string(core_groups[2],buf), match2string(core_groups[3],buf), match2string(core_groups[4],buf));

			temp = match2string(core_groups[1],buf);
			(steps[i])->pc = strtol(temp, NULL, 0);
			free(temp);
			(steps[i])->instr = match2string(core_groups[2],buf);

			//Convert each following token into its appropriate data type (instead of string)
			//Save the data in the correct struct variable
			token = strtok(match2string(core_groups[3],buf)," \n\t,=:");
			for( j=0; ; j++)
			{
				(steps[i])->(outputs[j])->type = token;
				token = strtok(match2string(NULL," \n\t,=:");
				(steps[i])->(outputs[j])->val = strtol(token, NULL, 0);
				token = strtok(match2string(NULL," \n\t,=:");
				if(token==NULL)
					break;
			}
			(steps[i])->out_size = j;	
			token = strtok(match2string(core_groups[4],buf)," \n\t,=:");
			for( j=0; ; j++)
			{
				(steps[i])->(inputs[j])->type = token;
				token = strtok(match2string(NULL," \n\t,=:");
				(steps[i])->(inputs[j])->val = strtol(token, NULL, 0);
				token = strtok(match2string(NULL," \n\t,=:");
				if(token==NULL)
					break;
			}
			(steps[i])->in_size = j;

			i++;
		}
	
		else if (core_reti != REG_NOMATCH) {
			fprintf(stderr, "Regex match failed: %s\n", buf);
			exit(1);
		}
	}
	return 0;
}
