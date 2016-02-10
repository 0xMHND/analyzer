#include <stdlib.h>
#include <stdio.h>

#define CONFIG "visualization.cfg"

int plot_data(uint64_t * xvals, uint64_t * yvals, int size)
{
	char buf[BUF_SIZE];

	FILE * temp = fopen("chart.dat", "w");
	if (temp == NULL) {
		fprintf(stderr,"Could not open/create chart.dat file.\n");
		return 1;
	}

	for (int i=0; i < size; i++) {
		fprintf(temp, "%ld %ld\n", xvals[i], yvals[i]); //Write the data to a temporary file
	}
	fclose(temp);

	FILE * cfg = fopen(CONFIG, "r");
	if (cfg == NULL) {
		fprintf(stderr,"Could not open %s.\n", CONFIG);
		return 1;
	}
	
	FILE * gnuplotPipe = popen ("gnuplot", "w");// -persistent", "w");
	if (gnuplotPipe == NULL) {
		fprintf(stderr,"Could not open GNUplot.\n");
		return 1;
	}

	fprintf(gnuplotPipe, "%s\n", "set terminal png truecolor"); //Send commands to gnuplot one by one.
	fprintf(gnuplotPipe, "%s\n", "set output \'image.png\'");
	while (!feof(cfg)) {
		memset(buf, '\0', BUF_SIZE);
		fgets(buf, BUF_SIZE, cfg);

		if (feof(cfg))
			break;

//		if (buf[0] != '#')	
			fprintf(gnuplotPipe, "%s\n", buf);
	}
	fprintf(gnuplotPipe, "%s\n", "plot \'chart.dat\'");

	pclose(gnuplotPipe);
	return 0;
}
