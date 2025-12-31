#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

int main() {
    long long sum = 0;
    int x;

    double t0 = hpc_now();
    while (scanf("%d", &x) == 1)
        sum += x;
    double t1 = hpc_now();

    printf("time=%f sum=%lld\n", t1 - t0, sum);
}