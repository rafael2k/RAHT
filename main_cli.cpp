#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cstring>

#include "haar3D.h"
#include "file/file.h"

#define MAX_FILENAME 512
#define DEFAULT_IO_BUFFER_SIZE 1024

// Point Cloud auxiliary functions
size_t get_voxel_number_xyzrgb(char *filename);
bool write_xyzrgb(char *filename, size_t voxel_number, double *V, double *C);
bool read_xyzrgb(char *filename, size_t voxel_number, double *V, double *C);

int scale = 5000;

// raht_enc main()
#ifdef RAHT_ENC
int main(int argc, char *argv[]){

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s input_file.xyzrgb output_file.raht\n\n", argv[0]);
        return EXIT_SUCCESS;
    }

    size_t voxel_number = get_voxel_number_xyzrgb(argv[1]);
    fprintf(stderr, "voxel numbers = %lu\n", voxel_number);

    double *inV = (double *) calloc (voxel_number, sizeof(double)*3);
    double *inC = (double *) calloc (voxel_number, sizeof(double)*3);
    intmax_t *data = (intmax_t *) calloc(voxel_number, sizeof(intmax_t) * 3);

    read_xyzrgb(argv[1], voxel_number, inV, inC);

    int64_t Qstep = 10 << INTEGER_STEPSIZE_PRECISION;
    size_t K = 3; // color components
    size_t depth = 6; // octree depth how much to use here?

    haar3D(Qstep, inV, inC, K, voxel_number, depth, data);

    // TODO: write header
    // rlgr coder
    file *rlgr_out = new file(argv[2], 1);
    rlgr_out->rlgrWrite(data, voxel_number, 0); // should I multiply voxel_number by 3?


    // TEST
    // rlgr decoder
    file rlgr_in(argv[2], 0);
    intmax_t *data_r = (intmax_t *) calloc(voxel_number, sizeof(intmax_t) * 3);

    rlgr_in.rlgrRead(data_r, voxel_number * 3, 0);

    double *CT = (double *) calloc(voxel_number, sizeof(double) * 3);

    for(size_t i = voxel_number; i > 0; i-- )
        CT[i] = data_r[i];

    double *data_out = (double *) calloc (voxel_number, sizeof(double) * 3);
    inv_haar3D(Qstep, inV, CT, K, voxel_number, depth, data_out); // lets understand this first

    write_xyzrgb("out.xyzrgb", voxel_number, inV, data_out);

    free(inV);
    free(inC);
    free(data);

    // decoder
    free(data_r);
    free(CT);
    free(data_out);
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


    return EXIT_SUCCESS;
}

#endif


// Point Cloud auxiliary functions

bool read_xyzrgb(char *filename, size_t voxel_number, double *inV, double *inC)
{
    char line_buffer[DEFAULT_IO_BUFFER_SIZE];
    double x, y, z, r, g, b;

    FILE *file = fopen(filename, "r");

    if ( file == NULL )
    {
        fprintf(stderr, "I/O error.\n");
        return false;
    }

    size_t counter = 0;
    while (fgets(line_buffer, DEFAULT_IO_BUFFER_SIZE, file)) {
        if (sscanf(line_buffer, "%lf %lf %lf %lf %lf %lf",
                &x, &y, &z, &r, &g, &b) == 6) {

            // scale up?
            x *= scale;
            y *= scale;
            z *= scale;

            inV[counter] = x;
            inV[counter + voxel_number] = y;
            inV[counter + (voxel_number * 2)] = z;

            // scale up color range too? (nope, it segfaults)
//            r *= 255;
//            g *= 255;
//            b *= 255;

            printf("%f %f %f %f %f %f\n", x, y, z, r, g, b);

            inC[counter] = r;
            inC[counter + voxel_number] = g;
            inC[counter + (voxel_number*2)] = b;
            counter++;
        }
    }

    fclose(file);

    return true;
}

bool write_xyzrgb(char *filename, size_t voxel_number, double *V, double *C)
{
    FILE *file = fopen(filename, "w");

    if ( file == NULL )
    {
        fprintf(stderr, "I/O error.\n");
        return false;
    }

    for (size_t i = 0; i < voxel_number; i++) {

        if (fprintf(file, "%.10f %.10f %.10f %.10f %.10f %.10f\n",
                V[i] / scale, V[i+voxel_number] / scale, V[i + (voxel_number*2)] / scale,
                C[i], C[i+voxel_number], C[i + (voxel_number*2)]) < 0) {
            fprintf(stderr, "I/O error writing XYZRGB file.\n");
            fclose(file);
            return false;
        }
    }

    fclose(file);
    return true;
}

size_t get_voxel_number_xyzrgb(char *filename){
    size_t counter = 0;

    FILE *fp_in = fopen(filename, "r");
    if (fp_in == NULL)
        return 0;

    int c;
    // We need the last \n before EOF!!
    while ( (c = fgetc (fp_in)) != EOF)
    {
        if ((char) c == '\n')
            counter++;
    }

    fclose(fp_in);
    return counter;
}
