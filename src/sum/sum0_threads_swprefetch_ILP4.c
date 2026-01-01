#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "timer.h"

#define ARR_SIZE	(1024*1024*1024)
#define THREADS 8
#define PREFETCH_DISTANCE (128) // Macbook Pro M2 has 128 byte cache-line

int *arr;
int size;

struct thread_args{
    int tid;
	long long ret;
};

void *worker(void *arg) {

	struct thread_args *args = arg;

	int tid = args->tid;
	long long sum = 0;

	int elems_per_thread = (size + THREADS - 1) / THREADS;

	int start = tid * elems_per_thread;
	int end = start + elems_per_thread;
	if(start + elems_per_thread > size) end = size;

	for(int i = start; i + 3 < end; i+=4) {
		__builtin_prefetch(&arr[i + PREFETCH_DISTANCE], 0, 3); // current cache-line
		sum += arr[i];
		sum += arr[i+1];
		sum += arr[i+2];
		sum += arr[i+3];
	}

	args->ret = sum;

    return NULL;
}

int main() {
    long long sum = 0;
    arr = (int*) malloc(sizeof(int) * ARR_SIZE);
    if(!arr) {
		printf("ENOMEM: arr\n");
		return 1;
	}

	int x;
	int i = 0;
    while (scanf("%d", &x) == 1) {
		arr[i++] = x;
		if(i == ARR_SIZE) break;
	}
	size = i;

	pthread_t threads[THREADS];
    struct thread_args args[THREADS];

    double t0 = hpc_now();
	
	for (int i = 0; i < THREADS; i++) {
        args[i].tid = i;
        pthread_create(&threads[i], NULL, worker, &args[i]);
    }

	for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
		sum += args[i].ret;
    }

    double t1 = hpc_now();

    printf("time=%f sum=%lld\n", t1 - t0, sum);
}
