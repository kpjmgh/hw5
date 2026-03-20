#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sobel.h"
#include "rtclock.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Sobel kernels
int Kx[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

int Ky[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

// Globals: Image and threading data
unsigned char **input_image;
unsigned char **output_image;
unsigned char threshold = 127;
int width, height;
int num_threads;
const int sequential = 0;//added by me 
const int parallel = 1;//added by me
int mode = sequential; //added by me
//int size;//added by me

/**
 * Main method
 */
int main(int argc, char *argv[]) {

    // TODO - Handle command line inputs
    if (argc < 4) {
        printf("Usage: ./sobel <input-file> <num-threads (>= 1)> <threshold (0-255)> \n");
        return -1;
    }

    char *filename = argv[1];
    //num_threads = argv[2];
    sscanf(argv[2], "%d", &num_threads);
    //threshold = (unsigned char)argv[3];
    //threshold = (int)argv[3];
    threshold = (int)(intptr_t)argv[3];

    // TODO - Read image file into array a 1D array (see assignment write-up)
    unsigned char *data = stbi_load(filename, &width, &height, NULL, 1); //taken from assignment

    printf("Loaded %s. Height=%d, Width=%d\n", filename, height, width);

    //remapping
    input_image = (unsigned char**) malloc(sizeof(unsigned char*) * height);
    for(int i = 0; i < height; i++){
        input_image[i] = &data[i * width]; //assign each row the proper pixel offset
    }

    //malloc output image array
    output_image = malloc(sizeof(unsigned char*) * height);//malloc a size 'height' array of pointers (these are the rows)
    for (int i = 0; i < height; i++){//iterate through each row and malloc an array of size 'width'
        //output_image[i] = malloc(sizeof(int) * width); 
        output_image[i] = calloc(width, sizeof(int)); //I was having issues here and using 'calloc' instead of 'malloc' seemed to fix them
    }//now we have a 2D array, acessible via output_image[x][y]



    // Start clocking! //tracks how long it takes
    double startTime, endTime;
    startTime = rtclock();



    // TODO - Prepare and create threads
    if(mode == parallel){
        seqSobel();
    }
    
    if(mode == sequential){
        //detect edges
    }

    // TODO - Wait for threads to finish
    
    // End clocking!
    endTime = rtclock();
    printf("Time taken (thread count = %d): %.6f sec\n", num_threads, (endTime - startTime));

    // TODO - Save the file!
    unsigned char *array1D = malloc(width * height * sizeof(unsigned char));
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            array1D[i * width + j] = output_image[i][j];
        }
    }
    char *outfilename = "outfile";
    stbi_write_jpg(outfilename, width, height, 1, array1D, 80);

    // TODO - Free allocated memory
    free(array1D);
    free2Darray(input_image);
    free2Darray(output_image);


    return 0;
}

