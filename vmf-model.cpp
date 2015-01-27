
// (C) Copyright 2013, Khoat Than (khoattq [at] soict.hust.edu.vn)

// This file is part of VMF.

// VMF is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.

// VMF is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA

#include "vmf-model.h"

VMF_model* VMF_model_New(int num_terms, int num_topics) {
    VMF_model* model;

    
    // init vmf_model
    model = (VMF_model*) malloc(sizeof (VMF_model));
    if (model == 0) {
        printf("\n VMF_model_New: cannot allocate memory for the model.\n");
        exit(0);
    }
    
    
    model->num_topics = num_topics;
    model->num_terms = num_terms;
    
    

    
    
    // khoi tao muy va kappa cua K - cao na
    // khoi tao muy va kappa cua VMF_model
    // da them khoi tao 
    model->kappa=(double*) malloc(num_topics*sizeof(double));
    model->muy=(double**) malloc(num_topics*sizeof(double*));
    
    for (int i=0; i<num_topics; i++){
        model->muy[i]=(double*) malloc(num_terms*sizeof(double));
    }
        
    



    // giu nguyen
    model->inv_sigma = initialize_matrix(num_topics - 1, num_topics - 1);

    // giu nguyen
    model->inv_sigma_sum = (double*) malloc(sizeof (double)*(num_topics - 1));

    // giu nguyen
    model->mu = (double*) malloc(sizeof (double)*(num_topics - 1));

    // giu nguyen
    if (model->mu == 0 || model->inv_sigma == 0 || model->inv_sigma_sum == 0 ) {
        printf("\n VMF_model_New: cannot allocate memory.\n");
        exit(0);
    }
    return (model);
}

void VMF_model_Free(VMF_model* model) {
    int i;
    //for (i = model->num_topics -1; i>=0; i--)	free(model->bb[i]);        
    for (i = model->num_topics - 2; i >= 0; i--) {
        free(model->inv_sigma[i]);
    }
    
    for (int i=0; i<model->num_topics; i++){
        free(model->muy[i]);
    }
    
    free(model->muy);
    free(model->kappa);
    
    free(model->mu);
    free(model->inv_sigma_sum);
    free(model);
}
/*
 * save an VMF model
 *
 */

void VMF_model_Save(VMF_model* model, char* model_root) {
    char filename[1000];
    FILE *fileptr;
    int i, j;

    sprintf(filename, "%s.other", model_root);
    fileptr = fopen(filename, "w");
    fprintf(fileptr, "num_topics: %d \n", model->num_topics);
    fprintf(fileptr, "num_terms: %d \n", model->num_terms);
    fprintf(fileptr, "log_det_cov: %f \n", model->log_det_cov);
    fprintf(fileptr, "stat: %f \n", model->stat);
    fprintf(fileptr, "lambda: %f \n", LAMBDA);
    fprintf(fileptr, "iterations for inference: %d \n", INF_MAX_ITER);
    fclose(fileptr);
    sprintf(filename, "%s.beta", model_root);
    fileptr = fopen(filename, "w");
    //save Topics
    
    
    
    // khong su dung cho nay
    // sua het
    // thay bang save muy va kappa
    for (i = 0; i < model->num_topics; i++) {
        for (j = 0; j < model->num_terms; j++)
            fprintf(fileptr, "%1.10f ", model->bb[i][j]);
        fprintf(fileptr, "\n");
    }
    
    // end
    
    
    
    
    fclose(fileptr);
    
    
    //save Mu
    // No edit
    sprintf(filename, "%s-mu.dat", model_root);
    fileptr = fopen(filename, "w");
    for (i = 0; i < model->num_topics - 1; i++)
        fprintf(fileptr, "%5.5f ", model->mu[i]);
    fclose(fileptr);
    
    
    //save Inv-sigma
    // No edit
    sprintf(filename, "%s-inv-sigma.dat", model_root);
    fileptr = fopen(filename, "w");
    for (i = 0; i < model->num_topics - 1; i++) {
        for (j = 0; j < model->num_topics - 1; j++)
            fprintf(fileptr, "%5.5f ", model->inv_sigma[i][j]);
        fprintf(fileptr, "\n");
    }
    fclose(fileptr);
}

VMF_model* VMF_model_Load(char* model_root) {
    char filename[1000];
    FILE* fileptr;
    VMF_model *model;
    int i, j, num_terms, num_topics;
    double x;

    sprintf(filename, "%s.other", model_root);
    fileptr = fopen(filename, "r");
    fscanf(fileptr, "num_topics: %d \n", &num_topics);
    fscanf(fileptr, "num_terms: %d \n", &num_terms);
    fclose(fileptr);

    sprintf(filename, "%s.beta", model_root);
    printf("loading %s\n", filename);
    fileptr = fopen(filename, "r");
    if (fileptr == NULL) {
        printf("Cannot open %s\n", filename);
        exit(0);
    }

    model = VMF_model_New(num_terms, num_topics);
    
    
    // Sua code nay
    // Thay bang doc kappa va muy
    // read Topics
    
    
    for (i = 0; i < num_topics; i++)
        for (j = 0; j < num_terms; j++) {
            fscanf(fileptr, "%lf", &x);
            if (x > 1e-10) model->bb[i][j] = x;
            else model->bb[i][j] = 1e-10;
        }
    fclose(fileptr);

    
    // giu nguyen
    //read Mu
    sprintf(filename, "%s-mu.dat", model_root);
    fileptr = fopen(filename, "r");
    if (fileptr == NULL) {
        printf("Cannot open %s\n", filename);
        exit(0);
    }
    for (i = 0; i < num_topics - 1; i++) {
        fscanf(fileptr, "%lf", &x);
        model->mu[i] = x;
    }
    fclose(fileptr);

    //read Inv-sigma
    sprintf(filename, "%s-inv-sigma.dat", model_root);
    fileptr = fopen(filename, "r");
    if (fileptr == NULL) {
        printf("Cannot open %s\n", filename);
        exit(0);
    }
    for (i = 0; i < num_topics - 1; i++)
        model->inv_sigma_sum[i] = 0;
    for (i = 0; i < num_topics - 1; i++)
        for (j = 0; j < num_topics - 1; j++) {
            fscanf(fileptr, "%lf", &x);
            model->inv_sigma[i][j] = x;
            model->inv_sigma_sum[j] += x;
        }
    fclose(fileptr);
    printf(" Model was loaded.\n");
    return (model);
}



// giu nguyen
// aa la topic cua moi document
void save_topic_docs_VMF(char* model_root, double **aa, corpus *corp, int num_topics) {
    char filename[1000];
    FILE *fileptr;
    int i, j;

    sprintf(filename, "%s-topics-docs-contribute.dat", model_root);
    fileptr = fopen(filename, "w");
    for (i = 0; i < corp->num_docs; i++) {
        if (aa[i][0] < 0) // in log-scale
            for (j = 0; j < num_topics; j++) aa[i][j] = exp(aa[i][j]);
        for (j = 0; j < num_topics; j++)
            fprintf(fileptr, "%1.10f ", aa[i][j]);
        fprintf(fileptr, "\n");
    }
    fclose(fileptr);
}



// Khoi tao muy va kappa, khong can sua
void initialize_M_step(VMF_model* model, corpus* corp) {
    int k, KK;
        
    initialize_random_topics(model);
    
    
    KK = model->num_topics - 1;
    for (k = 0; k < KK; k++) model->mu[k] = 0;
    //Diagonal Sigma
    for (k = 0; k < KK; k++) d_fill(model->inv_sigma[k], 0, KK);
    for (k = 0; k < KK; k++) {
        model->inv_sigma[k][k] = 1;
        model->inv_sigma_sum[k] = 1;
    }
    model->log_det_cov = 0;
    //for (k = 0; k < KK; k++) model->log_det_cov -= log(model->inv_sigma[k][k]);
    model->stat = -0.5 * (KK + model->log_det_cov) * corp->num_docs;
}




// Sua code nay
// Thay bang khoi tao muy va kappa
void initialize_random_topics(VMF_model *model) {
    int i, j;
    for (i = 0; i < model->num_topics; i++) {
        for (j = 0; j < model->num_terms; j++) model->bb[i][j] = rand() + 1;
        L1_normalize(model->bb[i], model->num_terms);
    }
}





void read_settings(char* filename) {
    FILE* fileptr;
    char *token, *str = (char*) malloc(2000);
    int n = 2000;
    const char delimiters[] = " \t";
    fileptr = fopen(filename, "r");
    getline(&str, &n, fileptr);
    token = strtok(str, delimiters);
    EM_MAX_ITER = atoi(token);
    getline(&str, &n, fileptr);
    token = strtok(str, delimiters);
    EM_CONVERGED = atof(token);
    getline(&str, &n, fileptr);
    token = strtok(str, delimiters);
    INF_MAX_ITER = atoi(token);
    getline(&str, &n, fileptr);
    token = strtok(str, delimiters);
    LAMBDA = atof(token);
    fclose(fileptr);
    free(str);
}


