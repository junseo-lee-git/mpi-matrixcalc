#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#define MATRIX_DIMENSION_XY 10

typedef unsigned short WORD; 
typedef unsigned int DWORD; 
typedef unsigned int LONG; 
typedef unsigned char BYTE;

typedef struct BITMAPFILEHEADER 
{ 
   WORD bfType;  //specifies the file type 
   DWORD bfSize;  //specifies the size in bytes of the bitmap file 
   WORD bfReserved1;  //reserved; must be 0 
   WORD bfReserved2;  //reserved; must be 0 
   DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits 
} BITMAPFILEHEADER; 

typedef struct BITMAPINFOHEADER 
{ 
    DWORD biSize;  //specifies the number of bytes required by the struct 
    LONG biWidth;  //specifies width in pixels 
    LONG biHeight;  //species height in pixels 
    WORD biPlanes; //specifies the number of color planes, must be 1 
    WORD biBitCount; //specifies the number of bit per pixel 
    DWORD biCompression;//spcifies the type of compression 
    DWORD biSizeImage;  //size of image in bytes 
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis 
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis 
    DWORD biClrUsed;  //number of colors used by th ebitmap 
    DWORD biClrImportant;  //number of colors that are important 
} BITMAPINFOHEADER; 

void test_func(int argc, char* argv[]){
    printf("argc:%d, ", argc);
    for(int i = 0; i < argc; i++){
        printf("%s", argv[i]);
        printf(", ");
    }
    printf("\n");
}

//SEARCH FOR TODO
//
//***********************************************************************************
//*************************************
// sets one element of the matrix

void set_matrix_elem(float *M,int x,int y,float f)
{
    M[x + y*MATRIX_DIMENSION_XY] = f;
}
//
//***********************************************************************************
//*************************************
// lets see if both are the same

int quadratic_matrix_compare(float *A,float *B)
{
    for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
        for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
        if(A[a +b * MATRIX_DIMENSION_XY]!=B[a +b * MATRIX_DIMENSION_XY]) 
            return 0;
    
    return 1;
}
//
//***********************************************************************************
//*************************************
//print a matrix
void quadratic_matrix_print(float *C)
{
    printf("\n");
    for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
        {
        printf("\n");
        for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
            printf("%.2f,",C[a + b* MATRIX_DIMENSION_XY]);
        }
    printf("\n");
}
    //
//***********************************************************************************
//*************************************
// multiply two matrices
void quadratic_matrix_multiplication(float *A,float *B,float *C)
{
//nullify the result matrix first
    for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
        for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
            C[a + b*MATRIX_DIMENSION_XY] = 0.0;
    //multiply
    for(int a = 0;a<MATRIX_DIMENSION_XY;a++) // over all cols a
        for(int b = 0;b<MATRIX_DIMENSION_XY;b++) // over all rows b
            for(int c = 0;c<MATRIX_DIMENSION_XY;c++) // over all rows/cols left
                {
                    C[a + b*MATRIX_DIMENSION_XY] += A[c + b*MATRIX_DIMENSION_XY] * B[a 
    + c*MATRIX_DIMENSION_XY]; 
                }
}
//
//***********************************************************************************
//*************************************
void synch(int par_id,int par_count,int *ready, int gate)
{
    //synch algorithm. make sure, ALL processes get stuck here until all ARE here
    ready[par_id]++;
    int breakout = 1;
    while(breakout){
        breakout = 0;
        for (int i = 0; i < par_count; i ++){
            if (ready[i] < gate)
                breakout = 1;
        }
    }

    // ready[par_id]--;
}
    //
// ***********************************************************************************
// *************************************

int main(int argc, char *argv[])
{
    int par_id = 0; // the parallel ID of this process
    int par_count = 1; // the amount of processes

    float *matA,*matB,*matC; //matrices A,B and C
    int *ready; //needed for synch
    int imagesize = 0;
    int *sharedsize;
    int *width;
    int pad;


    BITMAPFILEHEADER fh1;
    BITMAPINFOHEADER fih1;
    BITMAPFILEHEADER fh2;
    BITMAPINFOHEADER fih2;


    time_t t;

    struct timeval diff, ta, tb;

    printf("\n");
    test_func(argc, argv);

    if(argc!=3)
    {
        printf("no shared process\n");
    }
    else
    {
        par_id= atoi(argv[1]);
        par_count= atoi(argv[2]);
        // strcpy(shared_mem_matrix,argv[3]);
    }
    
    if(par_count==1){printf("only one process\n");}
    int fd[6];

    // read and manage bitmap data
    if(par_id==0)
    {
    FILE *f1 = fopen("f0.bmp", "rb");
    FILE *f2 = fopen("f2.bmp", "rb");

    if (f1){
        fread(&fh1.bfType, 2, 1, f1);
        fread(&fh1.bfSize, 4, 1, f1);
        fread(&fh1.bfReserved1, 2, 1, f1);
        fread(&fh1.bfReserved2, 2, 1, f1);  // read header data
        fread(&fh1.bfOffBits, 4, 1, f1);

        fread(&fih1, sizeof(fih1), 1, f1);  // read info header data
    }

    if (f2){
        fread(&fh2.bfType, 2, 1, f2);
        fread(&fh2.bfSize, 4, 1, f2);
        fread(&fh2.bfReserved1, 2, 1, f2);
        fread(&fh2.bfReserved2, 2, 1, f2);  // read header data
        fread(&fh2.bfOffBits, 4, 1, f2);

        fread(&fih2, sizeof(fih2), 1, f2);  // read info header data
    }

    imagesize = fih1.biSizeImage; // update image size

    //init the shared memory for A,B,C, ready. shm_open with C_CREAT here! 
    // then ftruncate! then mmap
    fd[0] = shm_open("matrixA", O_CREAT | O_RDWR, 0777);
    fd[1] = shm_open("matrixB", O_CREAT | O_RDWR, 0777);
    fd[2] = shm_open("matrixC", O_CREAT | O_RDWR, 0777);
    fd[3] = shm_open("synchobject", O_CREAT | O_RDWR, 0777);
    fd[4] = shm_open("sharedsize", O_CREAT | O_RDWR, 0777);
    fd[5] = shm_open("quadratic", O_CREAT | O_RDWR, 0777);

    ftruncate(fd[0], sizeof(float) * imagesize);  // matrix a
    ftruncate(fd[1], sizeof(float) * imagesize);  // matrix b
    ftruncate(fd[2], sizeof(float) * imagesize);  // matrix c
    ftruncate(fd[3], 1000);  // ready
    ftruncate(fd[4], 4);  // shared size
    ftruncate(fd[5], 4);  // dimensions

    matA = (float*) mmap(NULL, imagesize * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0);
    matB = (float*) mmap(NULL, imagesize * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[1], 0);
    matC = (float*) mmap(NULL, imagesize * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[2], 0);
    ready = (int*) mmap(NULL, par_count * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[3], 0);
    sharedsize = (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[4], 0);
    width = (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[5], 0);

    *sharedsize = imagesize;
    *width = fih1.biWidth;

    for (int i = 0; i < par_count; i++){  // set all ready ints to 0
        ready[i] = 0;
    }

    sbrk(3* imagesize);
    BYTE f1data[imagesize];
    BYTE f2data[imagesize];


    fread(f1data, 1, imagesize, f1);  // create unsigned char pointer for data
    fread(f2data, 1, imagesize, f2);

    // normalize colors then store into matA, matB, and clear matC
    for (int i = 0; i < imagesize; i++){
        float tmp1 = (float) f1data[i];
        float tmp2 = (float) f2data[i];
        matA[i] = tmp1 / 255;
        matB[i] = tmp2 / 255;
        matC[i] = 0;
    }

    fclose(f1);
    fclose(f2);

    }
    else
    {
    //init the shared memory for A,B,C, ready. shm_open withOUT O_CREAT 
    //here! NO ftruncate! but yes to mmap
    // printf("test0: %d\n", par_id);
    sleep(3);
    fd[0] = shm_open("matrixA", O_RDWR, 0777);
    fd[1] = shm_open("matrixB", O_RDWR, 0777);
    fd[2] = shm_open("matrixC", O_RDWR, 0777);
    // printf("test1: %d\n", par_id);
    fd[3] = shm_open("synchobject", O_RDWR, 0777);
    fd[4] = shm_open("sharedsize", O_RDWR, 0777);
    fd[5] = shm_open("quadratic", O_RDWR, 0777);
    // printf("test2: %d\n", par_id);


    // printf("test1: %d\n", par_id);

    sharedsize = (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[4], 0);
    width = (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[5], 0);
    matA = (float*) mmap(NULL,  *sharedsize * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0);
        // printf("test3: %d\n", par_id);
    matB = (float*) mmap(NULL, *sharedsize * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[1], 0);
    matC = (float*) mmap(NULL, *sharedsize * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[2], 0);
    ready = (int*) mmap(NULL, 4 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[3], 0);
    // printf("test4: %d\n", par_id);
    sleep(2); //needed for initalizing synch
    }


    // printf("s1 enter: %d, ready in: %d, fd: %d\n", par_id, ready[par_id], fd[3]);
    synch(par_id,par_count,ready, 1);
    // printf("s1 exit: %d, ready out: %d, fd: %d\n", par_id, ready[par_id], fd[3]);



    int rbw = *width * 3;
    if ((*width * 3) % 4 != 0){
        rbw = (4 - (*width * 3)%4) + (*width * 3);
    }

    div_t d = div(*width, par_count);
    int remainder = d.rem;
    int quotient = d.quot;

//    printf("mult enter: %d, ready in: %d, fd: %d\n", par_id, ready[par_id], fd[3]);
    int offset = remainder;
    // quadratic_matrix_multiplication_parallel(par_id, par_count,A,B,C, ...);
    int start, end;
    if (par_id == 0)
        start = 0;
        
    else
        start = quotient * par_id + offset;
    
    end = quotient * (par_id + 1) + offset;

    if(par_id == 0)
        gettimeofday(&ta, NULL);

    for(int a = 0;a<(*width);a++) // over all cols a
        for(int b = start;b<end;b++) // over all rows b
            for(int c = 0;c<(*width);c++) // over all rows/cols left
                {
                    // blue +0
                    matC[a*3 + b*rbw] += matA[c*3 + b*rbw] * matB[a*3 + c*rbw];

                    // green +1
                    matC[a*3 + b*rbw + 1] += matA[c*3 + b*rbw + 1] * matB[a*3 + c*rbw + 1];

                    // red +2
                    matC[a*3 + b*rbw + 2] += matA[c*3 + b*rbw + 2] * matB[a*3 + c*rbw + 2];
                }


    // printf("mult exit: %d, ready out: %d, fd: %d\n", par_id, ready[par_id], fd[3]);


    // printf("s3 enter: %d, ready in: %d, fd: %d\n", par_id, ready[par_id], fd[3]);
    synch(par_id,par_count,ready, 2);
    // printf("s3 exit: %d, ready out: %d, fd: %d\n", par_id, ready[par_id], fd[3]);

    if(par_id == 0){
        gettimeofday(&tb, NULL);
        int elapsed = ((tb.tv_sec - ta.tv_sec) * 1000000) + (tb.tv_usec - ta.tv_usec);
        printf("\nTime in microseconds: %d\n", elapsed);
    }

    if(par_id == 0){  // write data from matC to bmp file

        // copy matrix results and cast to BYTE
        BYTE result[imagesize];
        for(int i = 0; i < *sharedsize; i ++){
            result[i] = (BYTE) (matC[i] * 0.02 * 255);
        }

        FILE *out = fopen("outfile.bmp", "wb");
        fwrite(&fh1.bfType, 2, 1, out);
        fwrite(&fh1.bfSize, 4, 1, out);
        fwrite(&fh1.bfReserved1, 2, 1, out);
        fwrite(&fh1.bfReserved2, 2, 1, out);  // write header data
        fwrite(&fh1.bfOffBits, 4, 1, out);

        fwrite(&fih1, sizeof(fih1), 1, out);  // read info header data

        fwrite(result, 1, imagesize, out);  // write all pixel data points
        fclose(out);

        sbrk(0 - (3*imagesize));

    }

    // printf("synch4 enter: %d\n", par_id);
    synch(par_id, par_count, ready, 3);
    // printf("synch4 exit: %d\n", par_id);


    close(fd[0]);
    close(fd[1]);
    close(fd[2]);
    close(fd[3]);
    close(fd[4]);
    close(fd[5]);
    munmap(matA, sizeof(matA));
    munmap(matB, sizeof(matB));
    munmap(matC, sizeof(matC));
    munmap(sharedsize, sizeof(sharedsize));
    munmap(width, sizeof(width));
    munmap(ready, sizeof(ready));
    shm_unlink("matrixA");
    shm_unlink("matrixB");
    shm_unlink("matrixC");
    shm_unlink("synchobject");
    shm_unlink("sharedsize");
    shm_unlink("quadratic");
    return 0;    
}

// start time before the parallel and after the sync