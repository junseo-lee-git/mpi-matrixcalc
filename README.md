# mpi-matrixcalc
Project about practicing parallel processing in an Unix environment by writing a short MPI program and then allowing the user to specify how many cores to use, and matrix multiplying the pixel data of small bitmap files. The time taken to compute the resulting image is measured and printed.

**Running the code**

Program needs to be run in a unix environment since it involves unix kernel calls, I would recommend VirtualBox and using Ubuntu or Xubuntu.

Run the program succesfully by calling the mpi executable in commandline and providing the calc executable name, along with the number of cores/parallel processes that is desired.

EX: ./mpi calc 4

**Things to Note**

The images that are chosen to matrix multiple(f0, f1, f2) are currently hardcoded in the calc.c file, and can be changed on lines 176 and 177.

When testing matrix multiplication with different images, some combinations will result in pixel data result that exceed the 0-255 range for normal rgb values. This can be fixed by adjusting the float value on line 345.

**Credits**

Credits to Christian Eckhardt (ceckhard@calpoly.edu) for providing the inital quadratic_matrix_multiplication function on line 96 and guiding my thought process on the synch function.
