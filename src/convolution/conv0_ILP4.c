#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

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

void compute() {
	for(int i = 0; i < out->r; i++) {
		for(int j = 0; j < out->c; j++) {
			float sum = 0.0;
			for(int kr = 0; kr < kernel->r; kr++) {
				for(int kc = 0; kc + 3 < kernel->c; kc += 4) {
					sum += grid->data[(i + kr) * grid->c + (j + kc)] * kernel->data[kr * kernel->c + kc];
					sum += grid->data[(i + kr) * grid->c + (j + kc + 1)] * kernel->data[kr * kernel->c + kc + 1];
					sum += grid->data[(i + kr) * grid->c + (j + kc + 2)] * kernel->data[kr * kernel->c + kc + 2];
					sum += grid->data[(i + kr) * grid->c + (j + kc + 3)] * kernel->data[kr * kernel->c + kc + 3];
				}
			}
			out->data[i * out->c + j] = sum;
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
