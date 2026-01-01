#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));

    int rows = 100;
    int cols = 100;
    int k    = (int)rand() % 100;


    // ---- print input tensor ----
    printf("%d %d\n", rows, cols);
    for (int i = 0; i < rows * cols; i++) {
        float v = (float)rand() / RAND_MAX;
        printf("%f ", v);
    }
    printf("\n");

    // ---- print kernel ----
    printf("%d %d\n", k, k);
    for (int i = 0; i < k * k; i++) {
        float v = (float)rand() / RAND_MAX;
        printf("%f ", v);
    }
    printf("\n");

    return 0;
}
