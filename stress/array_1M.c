#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main() {
    int N = 1000 * 1000;

    /* strong-enough entropy for our use */
    unsigned seed = (unsigned)(time(NULL) ^ getpid());
    srand(seed);

    for (int i = 0; i < N; i++)
        printf("%d ", (int) rand() % 10);

    printf("\n");
    return 0;
}
