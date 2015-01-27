#include "utils.h"
#include <iostream>
#include <sys/types.h>

void make_directory(char* name)
{
    mkdir(name, S_IRUSR|S_IWUSR|S_IXUSR);
}

void L1_normalize(double *vec, int dim)
{
	int i;	double sum=0;
	for (i=0; i< dim; i++)	sum += vec[i];
	for (i=0; i< dim; i++)	vec[i] /= sum;
}

double** initialize_matrix(int rows, int cols)
{//initialize a matrix randomly
	int i, j, k;
	double **aa = (double**)malloc(sizeof(double*)*rows);
	if (aa == 0)
		{printf("\n initialize_matrix: failed to allocate memory! \n"); exit(0); } 
	for (i = 0; i < rows; i++)
	{
		aa[i] = (double*)malloc(sizeof(double)*cols);
		if (aa[i] == 0)
			{printf("\n initialize_matrix: failed to allocate memory 2! \n"); exit(0); } 
	}
	return (aa);
}

void d_fill(double *a, double value, int dim)
{	int i;
	for (i=0; i < dim; i++) a[i] = value;
}

void i_fill(int *a, int value, int dim)
{	int i;
	for (i=0; i < dim; i++) a[i] = value;
}

