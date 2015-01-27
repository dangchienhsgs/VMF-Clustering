#include "vmf-distribution.h"
#include <iostream>
#include "bessel.h"
#include <stdio.h>
#include <stdlib.h>
#include <NTL/RR.h>

using namespace NTL;
using namespace std;

#define E_RR to_RR("2.718281828459045")
#define DOUBLE_PI to_RR("6.283185307179586")
#define E 2.718281828459045

RR vmf_distribution(double* x, double* muy, double k) {
    int dim = sizeof (x) / sizeof (*x);
    int dim_muy = sizeof (muy) / sizeof (*muy);

    if (dim != dim_muy) {
        throw logic_error("Two vector do not have same dimension");
    } else {
        RR temp1;
        RR temp2;

        power(temp1, k, dim / 2 - 1);
        power(temp2, DOUBLE_PI, dim / 2);

        temp1 = temp1 / temp2;

        temp2 = BesselI(to_RR(dim / 2 - 1), k);
        temp1 = (temp1 / temp2);

                               
        //SparseMatrix temp_matrix = (SparseMatrix) muy.transpose() * x;
        RR product_vector=to_RR(0);
        for (int i=0; i<dim; i++){
            product_vector=product_vector+muy[i]*x[i];
        }

        temp2 = product_vector * k;
        temp2 = pow(E, conv<double> (temp2));


        temp1 = temp1*temp2;
        return temp1;
    }

}
