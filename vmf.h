
#ifndef VMF_H
#define VMF_H

typedef struct {
    double* mu; //Parameters for Gaussian // khong nen thay doi mu o code
    double** inv_sigma; //Parameters for Gaussian  // khong thay doi
    double* inv_sigma_sum; //sum of rows of Inv_sigma // khong thay doi
    double log_det_cov; // khong thay doi
    double stat; // khong thay doi	//-0.5 *(K-1) *log(2*pi) - 0.5*model->log_det_cov;

    //double** bb;			//topics
    double** muy; // phan voi vmf
    double* kappa; // phan voi vmf
    int num_topics;
    int num_terms;
} VMF_model;



#endif

