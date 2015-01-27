#include <stdio.h>
#include <math.h>
#include <float.h>
#include <iostream>
#include <stdlib.h>


void make_directory(char* name);

void L1_normalize(double *vec, int dim);

double** initialize_matrix(int rows, int cols);

void d_fill(double *a, double value, int dim);

void i_fill(int *a, int value, int dim);

#endif
