#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s ROWS COLS K\n", argv[0]);
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    int k    = atoi(argv[3]);   // kernel is k x k

    srand(time(NULL));

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
