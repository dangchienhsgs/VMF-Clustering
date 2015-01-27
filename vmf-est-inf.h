#ifndef VMF_EST_INF_H
#define VMF_EST_INF_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <time.h>

#include "vmf.h"
#include "data.h"
#include "utils.h"
#include "vmf-model.h"
#include "fw.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_eigen.h>

void VMF_Learn(char* directory, corpus* corp);

void M_step(VMF_model* model, corpus* corp, double **aa);

double det_inv_covariance(double **A, int dim);

double vMF_Infer(char *model_root, char *save, corpus* corp);

#endif
