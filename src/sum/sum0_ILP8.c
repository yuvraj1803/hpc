#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#define ARR_SIZE	(1024*1024*1024)
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
	
	for(i = 0; i + 7 < size; i+=8) {
		sum += arr[i];
		sum += arr[i+1];
		sum += arr[i+2];
		sum += arr[i+3];
		sum += arr[i+4];
		sum += arr[i+5];
		sum += arr[i+6];
		sum += arr[i+7];
	}

    double t1 = hpc_now();

    printf("time=%f sum=%lld\n", t1 - t0, sum);
}
