#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>


int main(int argc, char *argv[]){

    if (argc != 3){
        printf("Please enter a correct format: ./mpi calc #PAR_COUNT\n");
        exit(0);
    }

    char* args[4];
    args[0] = malloc(100);
    args[1] = malloc(100);
    args[2] = malloc(100);
    args[3] = NULL;

    int par_count = atoi(argv[2]);  // convert par_count to int

    char program[100];
    strcpy(program, "./");
    strcat(program, argv[1]);  // get program name
    strcpy(args[0], argv[1]);  // copy calc program name from command line
    strcpy(args[2], argv[2]);  // copy par count

    for(int i = 0; i < par_count; i++){  
        char text[100];
        sprintf(args[1], "%d", i);
        if (fork() == 0){
            execv(program, args);
            printf("execv not succesful\n");
        }

    }
    return 0;
}