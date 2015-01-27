
// (C) Copyright 2013, Khoat Than (khoattq [at] soict.hust.edu.vn)

// Online Frank-Wolfe algorithm, which is adapted to the inference problem in CTM


// This implementation is free for academic use.
// Any use for commercial purposes MUST ASK FOR PERMISSION.

#include "fw.h"
#include "vmf-distribution.h"
int T[2];

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Objective function is the joint P(d, theta)


/*********************************************************/
//Sua tai day

double f_joint(VMF_model *model, document *doc, double *theta, double* muy, double kappa) { // compute f(theta).		x = beta * theta
    int j, k, KK;
    double sum, fx = 0;


    // Sua phan nay, ta tinh log cua VMF cua doc, muy, kappa    
    // fx=f2=log VMF(doc|muy, kappa)
    /*****************************************************/
    fx = conv<double> (log(vmf_distribution(doc, muy, kappa)));    
    /*******************************************************/
    //



    // KK la tinh cua f1, khong can thiet phai sua
    KK = model->num_topics - 1;
    for (k = 0; k < KK; k++) {
        sum = 0;
        for (j = 0; j < KK; j++)
            sum += (theta[j] - theta[KK] - model->mu[j]) * model->inv_sigma[j][k];
        fx -= 0.5 * sum * (theta[k] - theta[KK] - model->mu[k]);
    }
    for (k = 0; k < KK + 1; k++) fx -= theta[k];
    return (fx);
}
/***************************************************************************/

/***
 * tinh dao ham
 */
double df_joint(VMF_model *model, document *doc, double *theta, int ind) {// compute dfx(theta)_ind.		x = beta * theta
    int j, KK;
    double sum, dfx = 0;

    //
    for (int i=0; i<model->num_topics; i++){        
        dfx=dfx+vmf_distribution(doc, model->muy[i], model->kappa[i])*theta[i];
    }
    
    dfx=vmf_distribution(doc, model->muy[ind], model->kappa[ind])/dfx;


        
        
        
    // khong phai sua gi ca
    // chu y xem random nhu the nao o dong code phia  duoi 
    
    dfx *= T[0];               
    KK = model->num_topics - 1;
    sum = -1;
    if (ind < KK)
        for (j = 0; j < KK; j++)
            sum -= model->inv_sigma[ind][j] * (theta[j] - theta[KK] - model->mu[j]);
    else
        for (j = 0; j < KK; j++)
            sum += model->inv_sigma_sum[j] * (theta[j] - theta[KK] - model->mu[j]);
    dfx += T[1] * (sum / exp(theta[ind]));
    return (dfx);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Objective function is the Likehood

double f_lkh(VMF_model *model, document *doc, double *theta, double *x) {
    // compute f(theta).		x = sum_k {beta_k * theta_k
    int j;
    double fx = 0;
    for (j = 0; j < doc->length; j++) fx += doc->counts[j] * log(x[j]);
    return (fx);
}

double df_lkh(VMF_model *model, document *doc, double *theta, double *x, int ind) {
    // compute dfx(theta)_ind.		x = sum_k {beta_k * theta_k
    int j;
    double dfx = 0;
    for (j = 0; j < doc->length; j++)
        dfx += doc->counts[j] * model->bb[ind][doc->words[j]] / x[j];
    return (dfx);
}


/********************************************************/
// Sua o day

double doc_projection(VMF_model* model, document *doc, double *theta,
        double (*compute_f)(VMF_model*, document*, double*, double, double),
        double (*compute_df)(VMF_model*, document*, double*, double*, int), double *lkh) {
    //Maximizing f(x) over the simplex of topics, using Online Frank-Wolfe
    //muy and kappa in vmf
    
    double obj, obj_max, fmax, alpha, muy, kappa, *theta_max, sum, EPS;
    int i, t, ind, no_improvement = 0;

    //opt = malloc(doc->length * sizeof(double));

    theta_max = malloc(model->num_topics * sizeof (double));


    EPS = log(1e-10);


    alpha = log(1 - (model->num_topics - 1) * exp(EPS));

    for (i = 0; i < model->num_topics; i++) theta[i] = EPS;
    // locate the initial point to be a vertex


    // Qua trinh nay la chon vector don vi nao sao cho f=f1+f2(theta=vectodonvi) lon nhat
    // luu vi tri cua 1-(k-1)10^-10 tai ind
    
    for (i = 0; i < model->num_topics; i++) {
        // Xet topic i
        theta[i] = alpha;

        muy = model->muy[i];
        kappa = model->kappa[i];

        // tinh lai ham f
        /***************/
        // Chu y check lai tham so xem co dung khong
        sum = (*compute_f)(model, doc, theta, muy, kappa);

        // 
        if (i == 0 || (sum > fmax)) {
            fmax = sum;
            ind = i;
        }
        theta[i] = EPS;
    }
    
    theta[ind] = alpha;
    
    /// theta bay gio la vector don vi thoa man dieu tren



    obj_max = fmax;

    // copy gia tri cua theta vao theta_max
    for (i = 0; i < model->num_topics; i++) theta_max[i] = theta[i];


    //for (i = 0; i < doc->length; i++)
    //    opt[i] = model->bb[ind][doc->words[i]];



    T[0] = 1;
    T[1] = 0;

    // Gieo l=1 den vo cung (tuc INF_MAX_ITER)    

    for (t = 1; t < INF_MAX_ITER; t++) {
        T[rand() % 2]++; //pick a part of the objective function (0/1)
        ind = -1; //select the best direction


        // Tinh dao ham cua F, tuc la tinh tat ca daohamcuaF[i]
        // df_joint phai sua
        // xac dinh duoc chieu ind thoa man daohamcuaF[ind] la max        
        
        for (i = 0; i < model->num_topics; i++) {
            sum = (*compute_df)(model, doc, theta, i);
            if (ind < 0 || (sum > fmax)) {
                fmax = sum;
                ind = i;
            }
        }


        // Cap nhat
        alpha = 2.0 / (t + 2);
        for (i = 0; i < model->num_topics; i++) theta[i] += log(1 - alpha);
        theta[ind] = log(exp(theta[ind]) + alpha);

        // Kiem tra xem co tang hay khong
        //for (i = 0; i < doc->length; i++)
        //    opt[i] = (1 - alpha) * opt[i] + alpha * model->bb[ind][doc->words[i]];
        obj = (*compute_f)(model, doc, theta, muy, kappa);
        //keep the best
        if (obj > obj_max) {
            obj_max = obj;
            no_improvement = 0;
            for (i = 0; i < model->num_topics; i++) theta_max[i] = theta[i];
        } else no_improvement++;

        // Neu qua 20 lan ko tang thi break
        if (no_improvement > 20) break;
    }



    //Reset Theta
    for (i = 0; i < model->num_topics; i++) theta[i] = theta_max[i];
    //for (i = 0; i < doc->length; i++) {
    //    opt[i] = 0;
    //    for (t = 0; t < model->num_topics; t++)
    //        opt[i] += exp(theta[t]) * model->bb[t][doc->words[i]];
    //}




    //Compute objective value and likelihood
    // Tinh lai ham muc tieu
    //for (i = 0; i < doc->length; i++) (*lkh) += doc->counts[i] * log(opt[i]);

    obj = (*compute_f)(model, doc, theta, muy, kappa);
    //free(opt);
    //free(theta_max);
    return (obj);
}

