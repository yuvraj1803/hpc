#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#define ARR_SIZE	(1024*1024*1024)
#define PREFETCH_DISTANCE (128) // Macbook Pro M2 has 128 byte cache-line
int *arr;

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
	int size = i;
    double t0 = hpc_now();	
	
	for(i = 0; i < size; i++) {
		__builtin_prefetch(&arr[i + PREFETCH_DISTANCE], 0, 3); // current cache-line
		sum += arr[i];
	}

    double t1 = hpc_now();

    printf("time=%f sum=%lld\n", t1 - t0, sum);
}
