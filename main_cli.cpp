#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cstring>

#include "haar3D.h"

#define MAX_FILENAME 512
#define DEFAULT_IO_BUFFER_SIZE 1024

// Point Cloud auxiliary functions
size_t get_voxel_number_xyzrgb(FILE *fp_in);
bool write_xyzrgb(FILE *file, size_t voxel_number, double *V, double *C);
bool read_xyzrgb(FILE *file, size_t voxel_number, double *V, double *C);

// raht_enc main()
#ifdef RAHT_ENC
int main(int argc, char *argv[]){

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s input_file.xyzrgb output_file.raht\n\n", argv[0]);
        return EXIT_SUCCESS;
    }

    FILE *fp_in = fopen(argv[1], "r");
    FILE *fp_out = fopen(argv[2], "w");

    if ( fp_in == NULL || fp_out == NULL)
    {
        fprintf(stderr, "I/O error.\n");
        return EXIT_FAILURE;
    }

    size_t voxel_number = get_voxel_number_xyzrgb(fp_in);
    fprintf(stderr, "voxel numbers = %lu\n", voxel_number);

    double *inV = (double *) calloc (voxel_number, sizeof(double)*3);
    double *inC = (double *) calloc (voxel_number, sizeof(double)*3);
    intmax_t *data = (intmax_t *) calloc(voxel_number, sizeof(intmax_t) * 3);

    read_xyzrgb(fp_in, voxel_number, inV, inC);

    int64_t Qstep = 1 << INTEGER_STEPSIZE_PRECISION;
    size_t K = 3; // color components
    size_t depth = 5; // octree depth how much to use here?

    haar3D(Qstep, inV, inC, K, voxel_number, depth, data);

    // write header

    // write output

    free(inV);
    free(inC);
    free(data);

    fclose(fp_in);
    fclose(fp_out);

    return EXIT_SUCCESS;
}

#endif

// raht_dec main()
#ifdef RAHT_DEC
int main(int argc, char *argv[]){

    if (argc < 3){
        fprintf(stderr, "Usage: %s input_file.raht output_file.xyzrgb\n\n", argv[0]);
        return EXIT_SUCCESS;
    }

    FILE *fp_in = fopen(argv[1], "r");
    FILE *fp_out = fopen(argv[2], "w");

    if ( fp_in == NULL || fp_out == NULL)
    {
        fprintf(stderr, "I/O error.\n");
        return EXIT_FAILURE;
    }

//TODO

    return EXIT_SUCCESS;
}

#endif


// Point Cloud auxiliary functions

bool read_xyzrgb(FILE *file, size_t voxel_number, double *inV, double *inC)
{
    char line_buffer[DEFAULT_IO_BUFFER_SIZE];
    double x, y, z, r, g, b;

    size_t counter = 0;
    while (fgets(line_buffer, DEFAULT_IO_BUFFER_SIZE, file)) {
        if (sscanf(line_buffer, "%lf %lf %lf %lf %lf %lf",
                &x, &y, &z, &r, &g, &b) == 6) {

            // scale up?
//            x *= 5000;
//           y *= 5000;
//            z *= 5000;
//            printf("%f %f %f %f %f %f\n", x, y, z, r, g, b);

            inV[counter] = x;
            inV[counter + voxel_number] = y;
            inV[counter + (voxel_number * 2)] = z;

            inC[counter] = r;
            inC[counter + voxel_number] = g;
            inC[counter + (voxel_number*2)] = b;
            counter++;
        }
    }

    return true;
}

bool write_xyzrgb(FILE *file, size_t voxel_number, double *V, double *C)
{
    for (size_t i = 0; i < voxel_number; i++) {

        if (fprintf(file, "%.10f %.10f %.10f %.10f %.10f %.10f\n",
                V[i], V[i+voxel_number], V[i + (voxel_number*2)],
                C[i], C[i+voxel_number], C[i + (voxel_number*2)]) < 0) {
            fprintf(stderr, "I/O error writing XYZRGB file.\n");
            fclose(file);
            return false;
        }
    }

    return true;
}

size_t get_voxel_number_xyzrgb(FILE *fp_in){
    size_t counter = 0;

    int c;
    // We need the last \n before EOF!!
    while ( (c = fgetc (fp_in)) != EOF)
    {
        if ((char) c == '\n')
            counter++;
    }

    fseek(fp_in, 0, SEEK_SET);
    return counter;
}
