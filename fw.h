#ifndef FW_H
#define FW_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "bessel.h"
#include "vmf-model.h"
#include "data.h"

double f_lkh(VMF_model *model, document *doc, double *theta, double *x);

double df_lkh(VMF_model *model, document *doc, double *theta, double *x, int ind);

double f_joint(VMF_model *model, document *doc, double *theta, double muy, double kappa);



double df_joint(VMF_model *model, document *doc, double *theta, double *x, int ind);

double doc_projection(VMF_model* model, document *doc, double *theta,
	double (*compute_f)(VMF_model*, document*, double*, double, double),
	double (*compute_df)(VMF_model*, document*, double*, double*, int), double *lkh);
#endif
