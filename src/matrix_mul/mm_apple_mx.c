#include "timer.h"
#include <stdio.h>
#include <Accelerate/Accelerate.h> 
#include <stdlib.h>

double time_begin;
double time_end;

struct tensor_2d {
	int r;
	int c;
	float *data;
};

struct tensor_2d *A;
struct tensor_2d *B;
struct tensor_2d *out;

struct tensor_2d *read_tensor() {
	struct tensor_2d *t = (struct tensor_2d *) malloc(sizeof(struct tensor_2d));
    scanf("%d %d", &t->r, &t->c);
    posix_memalign((void**)&t->data, 64, sizeof(float) * t->r * t->c);
    if (!t->data) {
        fprintf(stderr, "ENOMEM\n");
        exit(1);
    }

    for (int i = 0; i < t->r * t->c; i++)
        scanf("%f", &t->data[i]);

    return t;
}

void input() {
    A = read_tensor();
    B = read_tensor();

    if(!A || !B) {
        printf("error: could not read input tensors\n");
		exit(1);
    }

    if(A->c != B->r) {
        printf("error: A and B incompatible for matrix multiplication.\n");
        exit(1);
    }
}

void output() {
	printf("time: %f\n", (time_end - time_begin));
}

void precompute() {
    out = malloc(sizeof(struct tensor_2d));
	out->r = A->r;
	out->c = B->c;
    posix_memalign((void**)&out->data, 64, sizeof(float) * out->r * out->c);
}

void compute() {

    cblas_sgemm(
    CblasRowMajor, 
    CblasNoTrans,  
    CblasNoTrans,  
    A->r,          
    B->c,          
    A->c,          
    1.0f,          
    A->data,       
    A->c,          
    B->data,       
    B->c,          
    0.0f,          
    out->data,     
    out->c         
);

}

int main() {
	
	input();

	precompute();
	
	time_begin = hpc_now();
	compute();
	time_end = hpc_now();

	output();
}
