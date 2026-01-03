#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <arm_neon.h>

double time_begin;
double time_end;

struct tensor_2d {
	int r;
	int c;
	float *data;
};

struct tensor_2d *A;
struct tensor_2d *B;
struct tensor_2d *Bt;
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

struct tensor_2d *transpose_tensor(struct tensor_2d *A) {
    struct tensor_2d *T = (struct tensor_2d *) malloc(sizeof(struct tensor_2d));
    if(!T) {
        return NULL;
    }
    T->r = A->c;
    T->c = A->r;
    T->data = malloc(sizeof(float) * T->r * T->c);

    for(int i = 0; i < T->r; i++) {
        for(int j = 0; j < T->c; j++) {
            T->data[j * T->c + i] = A->data[i * A->c + j];
        }
    }

    return T;
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
}

void compute() {

    Bt = transpose_tensor(B);
    if(!Bt) {
        printf("ENOMEM: for transpose tensor\n");
        exit(1);
    }

    for(int i = 0; i < A->r; i++) {
        for(int j = 0; j < Bt->r; j++) {
			float32x4_t vsum = vdupq_n_f32(0.0f);

            for(int k = 0; k + 3 < A->c; k += 4) {
                float32x4_t va = vld1q_f32(&A->data[i * A->c + k]);
                float32x4_t vbt = vld1q_f32(&Bt->data[j * Bt->c + k]);

                vsum = vfmaq_f32(vsum, va, vbt);  
            }
            out->data[i * out->c + j] = vaddvq_f32(vsum);
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
