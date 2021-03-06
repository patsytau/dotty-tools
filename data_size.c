#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "utilities.h"
#include "macros.h"

/**
	this program reads a normal-augmented point cloud
	in "x y z nx ny nz" format and prints the extension
	of the point cloud in the x, y and z directions
*/
int main(int argc, char **argv)
{
	FILE * test_file = NULL;
	FILE * normed_file = NULL;

	double x, y, z;
	double nx, ny, nz;

	double max_x, max_y, max_z;
	double min_x, min_y, min_z;

	unsigned long int num_points = 0;
	int has_normals = 0;

	char *format_string = NULL;

	if( argc < 2 )
	{
		fprintf(stderr, "format is: %s <file-to-test> <opt: normalised-file>\n", argv[0] );
		return EXIT_FAILURE;
	}

	open_file(&test_file, argv[1], "r");
	if(argv[2])
		open_file(&normed_file, argv[2], "w");

	has_normals = detect_normals(test_file);

	if(has_normals)
		format_string = "%lf %lf %lf %lf %lf %lf";
	else
		format_string = "%lf %lf %lf";

	if( EOF == fscanf(test_file, format_string, &x, &y, &z, &nz, &ny, &nz) )
	{
		fprintf(stderr, "Error reading file, must exit.\n");
		fclose(test_file);
		return EXIT_FAILURE;
	}

	max_x = min_x = x;
	max_y = min_y = y;
	max_z = min_z = z;
	++num_points;

	while( EOF != fscanf(test_file, format_string, &x, &y, &z, &nz, &ny, &nz) )
	{
		max_x = max(x, max_x);
		max_y = max(y, max_y);
		max_z = max(z, max_z);

		min_x = min(x, min_x);
		min_y = min(y, min_y);
		min_z = min(z, min_z);

		++num_points;
	}

	/* will only be null here if no argument provided */
	if(normed_file != NULL)
	{
		/* get a uniform range of point data */
		double mid_x = (max_x + min_x)/2.0;
		double mid_y = (max_y + min_y)/2.0;
		double mid_z = (max_z + min_z)/2.0;

		rewind(test_file);

		/* if there are no normals, nx, ny and nz are ignored */
		while( EOF != fscanf(test_file, format_string, &x, &y, &z, &nx, &ny, &nz) )
		{
			fprintf(normed_file, "%f %f %f", x-mid_x, y-mid_y, z-mid_z);

			if(has_normals)
			{
				fprintf(normed_file, " %f %f %f", nx, ny, nz);
			}
			fprintf(normed_file, "\n");
		}
		fclose(normed_file);
	}

	printf("max_x = %f,\t min_x = %f\n", max_x, min_x);
	printf("max_y = %f,\t min_y = %f\n", max_y, min_y);
	printf("max_z = %f,\t min_z = %f\n", max_z, min_z);

	printf("x_range = %f\n", max_x - min_x);
	printf("y_range = %f\n", max_y - min_y);
	printf("z_range = %f\n", max_z - min_z);

	fclose(test_file);

	return EXIT_SUCCESS;
}
