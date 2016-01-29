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
#define NUM_COMMANDS 6

void plot_create_vs_time(struct DbCreate_info * c, int size)
{
    char * commandsForGnuplot[] = {"set terminal png truecolor", "set output \'create.png\'", "set style data lines", "set autoscale", "set title \'TITLEEEEE\'", "plot \'create.dat\'"};
	int xvals[size];
	int yvals[size];

	xvals[0] = c[0].time;
	yvals[0] = c[0].len;
	for (int i = 1; i < size; i++) {
		xvals[i] = c[i].time;
		yvals[i] = yvals[i-1] + c[i].len;
	}

	FILE * temp = fopen("create.dat", "w");
	for (int i=0; i < size; i++) {
		fprintf(temp, "%d %d\n", xvals[i], yvals[i]); //Write the data to a temporary file
	}
	fclose(temp);

	//Opens an interface that one can use to send commands as if they were typing into the
	//gnuplot command line.  "The -persistent" keeps the plot open even after your
	//C program terminates.
	FILE * gnuplotPipe = popen ("gnuplot", "w");// -persistent", "w");
	for (int i=0; i < NUM_COMMANDS; i++) {
		fprintf(gnuplotPipe, "%s\n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
	}
	pclose(gnuplotPipe);
}

void plot_usage_vs_time(struct DbCreate_info * c, int csize, struct DbDestroy_info * d, int dsize)
{
    char * commandsForGnuplot[] = {"set terminal png truecolor", "set output \'usage.png\'", "set style data lines", "set autoscale", "set title \'TITLEEEEE\'", "plot \'usage.dat\'"};
	int size = dsize + csize;
	int xvals[size];
	int yvals[size];
	int ccount = 1, dcount = 0;

	//we assume that a datablock is created first
	xvals[0] = c[0].time;
	yvals[0] = c[0].len;
	for (int i = 1; i < size; i++) {

		if(c[ccount].time < d[dcount].time && ccount < csize) {
			xvals[i] = c[ccount].time;
			yvals[i] = yvals[i-1] + c[ccount].len;
			ccount++;
		}
		else {
			xvals[i] = d[dcount].time;

			for (int j = 0; j < ccount; j++) {
				if (c[j].db == d[dcount].db) {
					yvals[i] = yvals[i-1] - c[j].len;
					break;
				}
			}
			dcount++;
		}	
	}

	FILE * temp = fopen("usage.dat", "w");
	for (int i=0; i < size; i++) {
		fprintf(temp, "%d %d\n", xvals[i], yvals[i]); //Write the data to a temporary file
	}
	fclose(temp);

	//Opens an interface that one can use to send commands as if they were typing into the
	//gnuplot command line.  Outputs result to an image (png) file.
	FILE * gnuplotPipe = popen ("gnuplot", "w");
	for (int i=0; i < NUM_COMMANDS; i++) {
		fprintf(gnuplotPipe, "%s\n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
	}
	pclose(gnuplotPipe);
}
