#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include "sobel.h"
// Note: You have access to all the global variables here.

// TODO - write your pthread-ready sobel filter function here for a range of rows

//making the sobel method sequentially first
void seqSobel(){
    
    //set output top row as 0
    populateZero(output_image[0], width);
    
    for(int x = 1; x<height-1; x++){
        //set first value as 0
        output_image[x][0] = 0;
        
        for(int y = 1; y<width-1; y++){
            int Gx = calculateGxy(x, y, Kx);
            int Gy = calculateGxy(x, y, Ky);
            
            int G = sqrt((Gx*Gx)+(Gy*Gy));

            output_image[x][y] = G;
        }
        
        //set last value as 0
        output_image[x][height] = 0;
    }
    
    //set output bottom row as 0
    populateZero(output_image[height-1], width);


    //replace all numbers less than threshold with 0
    considerThreshold(output_image);
}

//creating a parallel version
void parSobelThread(void* threadArgs){
    //struct arguments* args = (struct arguments*)threadArgs;
    struct arguments* args = (struct arguments*)malloc(sizeof(struct arguments));
    args = threadArgs;

    //arguments* args_ptr = (agruments*)threadArgs;
    int startIndex = args -> start;
    int endIndex = args -> end;
    //printf("Thread: %d. Start: %d. End: %d.\n", args -> tid, startIndex, endIndex);//testing

    for(int x = startIndex; x<endIndex-1; x++){
        output_image[x][0] = 0;
        
        for(int y = 1; y<width-1; y++){
            int Gx = calculateGxy(x, y, Kx);
            int Gy = calculateGxy(x, y, Ky);
            
            int G = sqrt((Gx*Gx)+(Gy*Gy));

            output_image[x][y] = G;
        }
        
        output_image[x][height] = 0;
    }
    
    considerThreshold(output_image);
    //free(args); //is this necessary?
    //printf("Thread %d Complete\n", args -> tid);//testing
}


//creating additional methods needed
int calculateGxy(int centeredX, int centeredY, int K[3][3]){
    int gArray[9];

    gArray[0] = input_image[centeredX-1][centeredY-1]*K[0][0];
    gArray[1] = input_image[centeredX-1][centeredY]*K[0][1];
    gArray[2] = input_image[centeredX-1][centeredY+1]*K[0][2];
    gArray[3] = input_image[centeredX][centeredY-1]*K[1][0];
    gArray[4] = input_image[centeredX][centeredY]*K[1][1];
    gArray[5] = input_image[centeredX][centeredY+1]*K[1][2];
    gArray[6] = input_image[centeredX+1][centeredY-1]*K[2][0];
    gArray[7] = input_image[centeredX+1][centeredY]*K[2][1];
    gArray[8] = input_image[centeredX+1][centeredY+1]*K[2][2];
    
    int g = gArray[0] + gArray[1] + gArray[2] + gArray[3] + gArray[4] + gArray[5] + gArray[6] + gArray[7] + gArray[8];
    return g;
}

void populateZero(unsigned char array[], int length){
    for(int i = 0; i<length; i++){
        array[i] = length;
    }
}

void considerThreshold(unsigned char **image){
    //replace all numbers less than threshold with 0
    for(int x = 1; x<height-1; x++){
        for(int y = 1; y<width-1; y++){
            if(output_image[x][y]<threshold){
                output_image[x][y] = 0;
            }
        }
    }
}

void free2Darray(unsigned char **arrayName){ 
    for(int i = 0; i <height; i++){//free each row
        free(arrayName[i]);//This line may be causing an issue for Valgrind
        arrayName[i] = NULL; //dangling pointer
    }
    free(arrayName);//free original array
    arrayName = NULL;
}

void printArray(unsigned char *arrayName){ //for testing
    printf("[");
    for(int i = 0; i <width; i++){//free each row
        printf("%d ", arrayName[i]);
    }
    printf("]");
}

void print2Darray(unsigned char **arrayName){ //for testing
    printf("2D Array:");
    for(int i = 0; i <height; i++){//free each row
        printArray(arrayName[i]);
        printf("\n");
    }
}