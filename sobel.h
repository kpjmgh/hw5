#define BLACK 0
#define WHITE 255

// These globals are declared here, but defined externally in another .c file
extern unsigned char **input_image;
extern unsigned char **output_image;
extern unsigned char threshold;
extern int width, height;
extern int num_threads;
extern int Kx[3][3];
extern int Ky[3][3];


// Thread function argument - TODO

// Function declarations - TODO
void seqSobel();
int calculateGxy(int centeredX, int centeredY, int K[3][3]);
void populateZero(int array[], int length);
void considerThreshold(unsigned char **image);
void free2Darray(unsigned char **arrayName);
