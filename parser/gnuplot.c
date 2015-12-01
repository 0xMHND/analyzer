/*
 * gnuplot.c
 *
 * The following code originally comes from the following site
 * http://stackoverflow.com/questions/3521209/making-c-code-plot-a-graph-automatically
 * It has been modified for our purposes
 */

#include <stdlib.h>
#include <stdio.h>

//following define is temporary
#define NUM_COMMANDS 2

void plot_create_vs_time(struct DbCreate_info * c, int size)
{
    char * commandsForGnuplot[] = {"set title \"Bytes Allocated vs Program ticks\"", "plot 'data.temp'"};
	int xvals[size];
	int yvals[size];

	xvals[0] = c[0].time;
	yvals[0] = c[0].len;
	for (int i = 1; i < size; i++) {
		xvals[i] = c[i].time;
		yvals[i] = yvals[i-1] + c[i].len;
	}

	FILE * temp = fopen("data.temp", "w");
	//Opens an interface that one can use to send commands as if they were typing into the
	//gnuplot command line.  "The -persistent" keeps the plot open even after your
	//C program terminates.
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");

	for (int i=0; i < size; i++) {
		fprintf(temp, "%d %d\n", xvals[i], yvals[i]); //Write the data to a temporary file
	}

	for (int i=0; i < NUM_COMMANDS; i++) {
		fprintf(gnuplotPipe, "%s\n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
	}
}
