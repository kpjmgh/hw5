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
int mode = parallel; //added by me
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
    //threshold = (int)(intptr_t)argv[3];
    threshold = atoi(argv[3]);

    // TODO - Read image file into array a 1D array (see assignment write-up)
    unsigned char *data = stbi_load(filename, &width, &height, NULL, 1); //taken from assignment. Valgrind seems to have an issue here
    

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
        output_image[i] = calloc(width, sizeof(unsigned char)); //I was having issues here and using 'calloc' instead of 'malloc' seemed to fix them
    }//now we have a 2D array, acessible via output_image[x][y]
    

    // Start clocking! //tracks how long it takes
    double startTime, endTime;
    startTime = rtclock();



    if(mode == sequential){
        seqSobel();
    }
    
    // TODO - Prepare and create threads
    if(mode == parallel){
        //printf("Entering parallel\n");//testing
        populateZero(output_image[0], width);    
        
        int average = height / num_threads;
        
        pthread_t workerThreads[num_threads]; //create worker threads
        int tid[num_threads]; //creates an empty array to be filled with thread ids for each thread
        

        for (int i = 0; i < num_threads; i++) { //for every thread:
            struct arguments *threadArgs = (struct arguments*)malloc(sizeof(struct arguments));
            //threadArgs = malloc(3 * sizeof(int));//this line is causing issues
            //threadArgs = calloc(3, sizeof(*threadArgs));

            threadArgs -> start = i * average; 
            threadArgs -> end = threadArgs -> start + average; 

            if(threadArgs -> start == 0){
                threadArgs -> start = 1;
            }
            if(i == num_threads-1){
                threadArgs -> end = height-2;//minus 2 because -1 should cause the final index number, and we need to stop one before it
            }

            tid[i] = i; //adds each thread id to the tid array 
            threadArgs -> tid = tid[i]; //there's an issue with this line somehow
            //threadArgs -> tid = i;
            pthread_create(&workerThreads[i], NULL, parSobelThread, (void*) threadArgs); //get the thread to actually do the task at hand//there seems to be a problem with what threadArgs is actually passing
            pthread_join(workerThreads[i], NULL);
            free(threadArgs);
        }
        
        /*
        // TODO - Wait for threads to finish
        //join threads
        for(int i = 0; i<num_threads; i++){
            pthread_join(workerThreads[i], NULL);
        }
        */
        populateZero(output_image[height-1], width);

    }

    
    // End clocking!
    endTime = rtclock();
    printf("Time taken (thread count = %d): %.6f sec\n", num_threads, (endTime - startTime));

    //printf("output_image: ");//testing
    //print2Darray(output_image);//testing

    // TODO - Save the file!
    unsigned char *array1D = malloc(width * height * sizeof(unsigned char));
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            array1D[i * width + j] = output_image[i][j];
        }
    }

    char *outfilename = "outfile.jpg";
    stbi_write_jpg(outfilename, width, height, 1, array1D, 80);

    // TODO - Free allocated memory
    free(array1D);
    free(input_image); //Valgrind seems to have an issue here.
    free2Darray(output_image);
    free(data);

    return 0;
}

