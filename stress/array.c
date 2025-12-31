#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s N\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);

    /* strong-enough entropy for our use */
    unsigned seed = (unsigned)(time(NULL) ^ getpid());
    srand(seed);

    for (int i = 0; i < N; i++)
        printf("%d ", (int) rand() % 10);

    printf("\n");
    return 0;
}
