#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s ROWS COLS. This program will generate two matrices (ROWSxCOLS) and (COLSxROWS)\n", argv[0]);
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    
    srand(time(NULL));

    // ---- print input tensor ----
    printf("%d %d\n", rows, cols);
    for (int i = 0; i < rows * cols; i++) {
        float v = (float)rand() / RAND_MAX;
        printf("%f ", v);
    }
    printf("\n");

    // ---- print kernel ----
    printf("%d %d\n", cols, rows);
    for (int i = 0; i < cols * rows; i++) {
        float v = (float)rand() / RAND_MAX;
        printf("%f ", v);
    }
    printf("\n");

    return 0;
}
