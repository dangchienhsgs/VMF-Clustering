// (C) Copyright 2013, Khoat Than (khoattq [at] soict.hust.edu.vn)

// This file is part of fCTM.

// fCTM is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.

// fCTM is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA

#include "vmf-est-inf.h"
#include "utils.h"

int LAG;


void VMF_Learn(char* directory, corpus* corp) {
    int d, i;
    VMF_model *model;
    double **aa, *fx, *dfx; // aa = log theta
    double jointProb, jointProb_old, converge_joint, likelihood;
    clock_t start, end, init = clock();
    char filename[1000];

    aa = initialize_matrix(corp->num_docs, NTOPICS); // new representation of documents ************giu nguyen
    // aa la log cua theta => giu nguyen
    // initialize model

    model = VMF_model_New(corp->num_terms, NTOPICS);

    initialize_M_step(model, corp);
    sprintf(filename, "%s/000-VMF", directory);
    VMF_model_Save(model, filename);

    // run EM algorithm to learn the model
    sprintf(filename, "%s/likelihood-VMF.dat", directory);
    FILE* jointProb_file = fopen(filename, "w");
    fprintf(jointProb_file, "Iteration, Likelihood, Objective, Converge-obj, Time (seconds) \n");
    printf("Learning...\n");
    LAG = 5;
    i = -1;
    jointProb_old = 0;
    
    
    // day la 2 ham f_joint va df_joint   
    // f_joint la thay cac vector don vi lan luot vao 
    // 
    fx = f_joint;
    dfx = df_joint;
    
    
    
    while (i < EM_MAX_ITER) {
        //do inference
        i++;
        printf("  **** iteration %d ****\n", i);
        start = clock();
        jointProb = model->stat;
        likelihood = 0;
        
        
        // cap nhat theta tuc aa
        // can sua code trong doc_projection
        for (d = 0; d < corp->num_docs; d++)
            jointProb += doc_projection(model, &(corp->docs[d]), aa[d], fx, dfx, &likelihood);
        end = clock();
        
        
        // Kiem tra xem phan E co hoi tu khong
        converge_joint = (jointProb - jointProb_old) / fabs(jointProb_old);
        jointProb_old = jointProb;
        printf("  Obj %f;  converge %2.5e;  Likelihood %f \n\n", jointProb, converge_joint, likelihood);
        fprintf(jointProb_file, "%d, %10.10f, %10.10f, %5.5e, %10.10f \n", i, likelihood,
                jointProb, converge_joint, ((double) (end - start)) / CLOCKS_PER_SEC);
        if ((i > 5) && (converge_joint < EM_CONVERGED)) break;
        //re-estimate the model
        M_step(model, corp, aa);
        // output model
        if ((i % LAG) == 0) {
            sprintf(filename, "%s/%03d-VMF", directory, i);
            VMF_model_Save(model, filename);
            save_topic_docs_VMF(filename, aa, corp, model->num_topics);
        }
    }
    // output the final model
    printf("    saving the final model.\n");
    sprintf(filename, "%s/final-VMF", directory);
    VMF_model_Save(model, filename);
    end = clock();
    fprintf(jointProb_file, "Overall time:, , , , %10.10f \n", ((double) (end - init)) / CLOCKS_PER_SEC);
    fclose(jointProb_file);
    
    
    
    //find independent representation for correlation exploration
    fx = f_lkh;
    dfx = df_lkh;
    for (d = 0; d < corp->num_docs; d++)
        doc_projection(model, &(corp->docs[d]), aa[d], fx, dfx, &likelihood);
    save_topic_docs_VMF(filename, aa, corp, model->num_topics);
    //free parameters
    for (d = corp->num_docs - 1; d >= 0; d--) free(aa[d]);
    VMF_model_Free(model);
    printf("    Model has been learned.\n");
}



void M_step(VMF_model* model, corpus* corp, double **aa) {//update the topics: topic[k][j] ~= sum_{d} aa[k][d] * N[d][j]
    int d, i, j, k, KK;
    double trace;

    KK = model->num_topics - 1;
    //compute Mu
    for (k = 0; k < KK; k++) {
        model->mu[k] = 0;
        for (d = 0; d < corp->num_docs; d++) model->mu[k] += aa[d][k] - aa[d][KK];
        model->mu[k] /= corp->num_docs;
    }
    //compute inv_sigma
    for (k = 0; k < KK; k++) d_fill(model->inv_sigma[k], 0, KK);

    for (k = 0; k < KK; k++)
        for (i = k; i < KK; i++)
            for (d = 0; d < corp->num_docs; d++)
                model->inv_sigma[k][i] += (aa[d][k] - aa[d][KK] - model->mu[k])*(aa[d][i] - aa[d][KK] - model->mu[i]);
    for (k = 0; k < KK; k++)
        for (i = k; i < KK; i++) {
            model->inv_sigma[k][i] *= 1.0 / corp->num_docs;
            model->inv_sigma[i][k] = model->inv_sigma[k][i];
        }
    if (LAMBDA > 0)
        for (k = 0; k < KK; k++) model->inv_sigma[k][k] += LAMBDA;

    model->log_det_cov = det_inv_covariance(model->inv_sigma, KK);
    for (k = 0; k < KK; k++) {
        model->inv_sigma_sum[k] = 0;
        for (i = 0; i < KK; i++) model->inv_sigma_sum[k] += model->inv_sigma[i][k];
    }
    
    
    
    //recover topic proportion
    for (d = 0; d < corp->num_docs; d++)
        for (k = 0; k < model->num_topics; k++) aa[d][k] = exp(aa[d][k]);
    // chu y, aa truoc do luu la log theta, sau buoc nay aa tro thanh theta
    
    
    
    
    /**********************************/
    //compute topics
    // sua code nay
    // cap nhat muy, kappa cua model    
    // cap nhat muy
    // muy[i]=Xichma theta[n, k] * x[n]
    
    
    // muy[i, j]=Sigma n=1-> number_document:  theta(n,i)*x[n, j]    
    for (int i=0; i<model->num_topics; i++){
        for (int j=0; j<model->num_terms; j++){                        
            
            model->muy[i][j]=0;
            for (int n=0; n<corp->num_docs; n++){
                
                // check if this doc contain the word j => add x[n, j], , else not contain x[n, j]=0
                bool check=false;
                int index_temp=0;                
                for (int index=0; index<sizeof(corp->docs[n]->words)/sizeof(int); index++){
                    if (corp->docs[n]->words[index]==j){
                        check=true;
                        index_temp=index;
                    }
                }
                
                if (check){
                    // this doc contain the word j-th
                    model->muy[i][j]=model->muy[i][j]+aa[n][i]*corp->docs[n]->counts[index_temp];
                }
            }                        
        }
        
        L1_normalize(model->muy[i], model->num_terms);        
    }
    
    
    
    // update kappa
    for (int i=0; i<model->num_topics; i++){
        double length_muy=0;
        double sum_theta=0;
        
        for (int j=0; j<model->num_terms; j++){
            length_muy=length_muy+model->muy[i][j];
        }
        
        
        for (int j=0; j<corp->num_docs; j++){
            sum_theta=sum_theta+aa[j][i];
        }
        
        double r=length_muy/sum_theta;
        model->kappa[i]=(r*model->num_terms-r*r*r)/(1-r*r);
    }
        
    
        
            
    /************************************/        
        
        
    // khong sua    
    trace = 0;
    for (k = 0; k < KK; k++) trace += model->inv_sigma[k][k];
    trace *= LAMBDA;
    model->stat = -0.5 * (trace + model->log_det_cov) * corp->num_docs;
}


// Khong dong vao code nay
// Tinh dinh thuc
double det_inv_covariance(double **A, int dim) {//The function computes inverse of A and stores inversion in A. It returns log[det(A)]
    gsl_matrix *evects, *mcpy;
    gsl_vector *evals;
    int k, i, j;
    double dete;
    gsl_eigen_symmv_workspace* wk;

    mcpy = gsl_matrix_alloc(dim, dim);
    evects = gsl_matrix_alloc(dim, dim);
    evals = gsl_vector_alloc(dim);
    wk = gsl_eigen_symmv_alloc(dim);
    //Eigen decomposition
    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++) gsl_matrix_set(mcpy, i, j, A[i][j]);
    i = gsl_eigen_symmv(mcpy, evals, evects, wk);

    gsl_eigen_symmv_free(wk);
    gsl_matrix_free(mcpy);

    double eva[dim], **V;
    V = initialize_matrix(dim, dim);
    dete = 0; //log[det(A)]
    for (i = 0; i < dim; i++) {
        eva[i] = gsl_vector_get(evals, i);
        dete += log(eva[i]);
        eva[i] = 1.0 / eva[i];
    }
    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++) V[i][j] = gsl_matrix_get(evects, i, j);
    //eigenvectors --> LU
    for (i = 0; i < dim; i++) eva[i] = sqrt(eva[i]);
    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++) V[j][i] *= eva[i];
    //inverse =  V*V^t
    for (i = 0; i < dim; i++)
        for (j = i; j < dim; j++) {
            A[i][j] = 0;
            for (k = 0; k < dim; k++) A[i][j] += V[i][k] * V[j][k];
            A[j][i] = A[i][j];
        }
    for (i = dim - 1; i >= 0; i--) free(V[i]);
    gsl_matrix_free(evects);
    gsl_vector_free(evals);
    return (dete);
}







/*
 * inference only
 *
 */

// Luu xn duoc bieu dien theo chu de ra file
// Phan nay su dung khi code da chay EM xong xuoi
double VMF_Infer(char *model_root, char *save, corpus* corp) {
    FILE* fileptr;
    char filename[1000];
    VMF_model *model;
    clock_t start, end, init;
    int d;
    double jointProb, likelihood, **aa, *fx, *dfx;

    init = clock();
    sprintf(filename, "%s-lhood.csv", save);
    fileptr = fopen(filename, "w");
    fprintf(fileptr, "docID, likelihood, jointProb, time \n");

    model = VMF_model_Load(model_root);
    aa = initialize_matrix(corp->num_docs, model->num_topics);

    printf(" Number of topics: %d \n", model->num_topics);
    printf(" Number of docs: %d \n", corp->num_docs);
    printf(" Infering...\n");
    fx = f_joint;
    dfx = df_joint;
    for (d = 0; d < corp->num_docs; d++) {
        if (((d % 500) == 0) && (d > 0)) printf("  document %d\n", d);
        start = clock();
        likelihood = 0;
        jointProb = doc_projection(model, &(corp->docs[d]), aa[d], fx, dfx, &likelihood);
        end = clock();
        fprintf(fileptr, "%d, %f, %f, %f \n", d, likelihood, jointProb, ((double) (end - start)) / CLOCKS_PER_SEC);
    }
    end = clock();
    fprintf(fileptr, "Overall time:, , , %f \n", ((double) (end - init)) / CLOCKS_PER_SEC);
    fclose(fileptr);
    save_topic_docs_VMF(save, aa, corp, model->num_topics);
    for (d = corp->num_docs - 1; d >= 0; d--) free(aa[d]);
    VMF_model_Free(model);
    printf("  Completed.\n");
    return (0);
}



