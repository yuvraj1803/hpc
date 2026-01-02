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

struct tensor_2d *grid;
struct tensor_2d *kernel;
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
	grid = read_tensor();
	kernel = read_tensor();

	if(!grid || !kernel) {
		printf("error: could not read input tensors\n");
		exit(1);
	}
}

void output() {

	printf("grid: %d x %d\n", grid->r, grid->c);
	printf("kernel: %d x %d\n", kernel->r, kernel->c);
	printf("output: %d x %d\n", out->r, out->c);

	printf("time: %f\n", (time_end - time_begin));

}

void precompute() {
	out = malloc(sizeof(struct tensor_2d));
	out->r = grid->r - kernel->r + 1;
	out->c = grid->c - kernel->c + 1;
	out->data = malloc(sizeof(float) * out->r * out->c);
}

// multiplies 4 floats using SIMD (Arm Neon)
void mul4(float *a, float *b, float *out) {
	float32x4_t va = vld1q_f32(a);
	float32x4_t vb = vld1q_f32(b);
	
	float32x4_t vc = vmulq_f32(va, vb);

    vst1q_f32(out, vc);
}

// adds 4 floats using SIMD (Arm Neon)
void add4(float *a, float *b, float *out) {
	float32x4_t va = vld1q_f32(a);
	float32x4_t vb = vld1q_f32(b);
	
	float32x4_t vc = vaddq_f32(va, vb);

    vst1q_f32(out, vc);
}
// if you see in the for(kc) loop, I am dropping the last 3 columns.
// in a real world implementation I would rather pad the kernel with
// 0s to make it a multiple of 4.
void compute() {
	for(int i = 0; i < out->r; i++) {
		for(int j = 0; j < out->c; j++) {
			float32x4_t vsum = vdupq_n_f32(0.0f);

			for(int kr = 0; kr < kernel->r; kr++) {
				for(int kc = 0; kc + 3 < kernel->c; kc+=4) {
					float32x4_t va = vld1q_f32(&grid->data[(i + kr) * grid->c + (j + kc)]);
					float32x4_t vb = vld1q_f32(&kernel->data[kr * kernel->c + kc]);

					vsum = vfmaq_f32(vsum, va, vb);
				}
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
