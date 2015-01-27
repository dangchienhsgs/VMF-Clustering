#ifndef VMF_MODEL_H
#define VMF_MODEL

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "vmf.h"
#include "data.h"
#include "utils.h"

#define myrand() (double) (((unsigned long) randomMT()) / 4294967296.)
#define NUM_INIT 1

float EM_CONVERGED;
int EM_MAX_ITER;
int NTOPICS;
int INF_MAX_ITER;
float LAMBDA;		// penalty on Tr(inv_sigma)

//=============================================
void VMF_model_Free(VMF_model* model);

void VMF_model_Save(VMF_model* model, char* model_root);

VMF_model* VMF_model_New(int num_terms, int num_topics);

VMF_model* VMF_model_Load(char* model_root);

void save_topic_docs_VMF(char* model_root, double **aa, corpus *corp, int num_topics);

void VMF_topics_Save(VMF_model* model, char* model_root);

void initialize_M_step(VMF_model* model, corpus* corp);

void initialize_random_topics(VMF_model *model);

void read_settings(char* filename);

#endif

