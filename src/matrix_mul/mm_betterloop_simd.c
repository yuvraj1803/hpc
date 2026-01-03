#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <arm_neon.h>
#include <memory.h>

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
    t->data = malloc(sizeof(float) * t->r * t->c);
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
	out->data = malloc(sizeof(float) * out->r * out->c);
    memset(out->data, 0, sizeof(float) * out->r * out->c);
}

/*
In the inner loop here, you will notice we will miss some columns
if B's columns number is not a multiple of 4.

Ofcourse in a real world implementation, padding B with a bunch of dummy 0 columns
would be a better idea than trying to add an ugly branch inside the loop. But here I
am assuming B has columns which are a multiple of 4.
*/

void compute() {

    for(int i = 0; i < A->r; i++) {
        for(int k = 0; k < A->c; k++) {
			float32x4_t va = vdupq_n_f32(A->data[i * A->c + k]);
            
            for(int j = 0; j + 3< B->c; j+=4) {
                float32x4_t vb = vld1q_f32(&B->data[k * B->c + j]);
                float32x4_t vout = vld1q_f32(&out->data[i * out->c + j]);        

                vout = vfmaq_f32(vout, va, vb);  
                
                vst1q_f32(&out->data[i * out->c + j], vout);            
            }
        }
    }
}

int main() {
	
	input();

	precompute();
	
	time_begin = hpc_now();
	compute();
	time_end = hpc_now();

	output();
}
