#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

double time_begin;
double time_end;

#define THREADS 10

struct thread_args {
    int tid;
};

struct tensor_2d {
	int r;
	int c;
	float *data;
};

struct tensor_2d *grid;
struct tensor_2d *kernel;
struct tensor_2d *out;

void *worker(void *arg) {
    struct thread_args *args = arg;

	int tid = args->tid;

	/*
		So the threading strategy we will be using here is:
			+ groups the rows into R rows per thread.
			+ Thread 0 will work on output rows 0...R-1
			+ Thread 1 will work on output rows R...2R-1
			...
			+ So how do we know what R is? Think of the case 
				when there is only 1 thread. Here R = out->r.
				When there are 2 threads, R = ceil(out->r / 2).
				So, wlog, when there are THREADS threads,
				R = ceil(out->r / THREADS)
			+ Now, given out thread ID, we will have to figure out
				which set of rows this thread will be working on.
	*/

	int R = (out->r + THREADS - 1) / THREADS;
	int row_start = tid * R;
	int row_end = row_start + R - 1;
	if(row_end >= out->r) row_end = out->r - 1;

	for(int i = row_start; i <= row_end; i++) {
		for(int j = 0; j < out->c; j++) {
			float sum = 0.0;
			for(int kr = 0; kr < kernel->r; kr++) {
				for(int kc = 0; kc < kernel->c; kc++) {
					sum += grid->data[(i + kr) * grid->c + (j + kc)] * kernel->data[kr * kernel->c + kc];
				}
			}
			out->data[i * out->c + j] = sum;
		}
	}

    return NULL;
}

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
 	pthread_t th[THREADS];
    struct thread_args arg[THREADS];

    for (int t = 0; t < THREADS; t++) {
        arg[t].tid = t;
        pthread_create(&th[t], NULL, worker, &arg[t]);
    }

    for (int t = 0; t < THREADS; t++)
        pthread_join(th[t], NULL);
}

int main() {
	
	input();

	precompute();

	time_begin = hpc_now();
	compute();
	time_end = hpc_now();

	output();
}
