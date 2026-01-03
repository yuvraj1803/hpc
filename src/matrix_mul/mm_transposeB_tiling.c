#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

double time_begin;
double time_end;

struct tensor_2d {
	int r;
	int c;
	float *data;
};

struct tensor_2d *A;
struct tensor_2d *B;
struct tensor_2d *Bt;
struct tensor_2d *out;

struct tensor_2d *transpose_tensor(struct tensor_2d *A) {
    struct tensor_2d *T = (struct tensor_2d *) malloc(sizeof(struct tensor_2d));
    if(!T) {
        return NULL;
    }
    T->r = A->c;
    T->c = A->r;
    T->data = malloc(sizeof(float) * T->r * T->c);

    for(int i = 0; i < T->r; i++) {
        for(int j = 0; j < T->c; j++) {
            T->data[j * T->c + i] = A->data[i * A->c + j];
        }
    }

    return T;
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
    A = read_tensor();
    B = read_tensor();

    if(!A || !B) {
        printf("error: could not read input tensors\n");
		exit(1);
    }

    if(A->c != B->r) {
        printf("error: A and B incompatible for matrix multiplication.\n");
        exit(1);
    }
}

void output() {
	printf("time: %f\n", (time_end - time_begin));
}

int TILE_ROWS;
int TILE_COLS;
int TILE_THRESHOLD = 64;

void precompute() {
    out = malloc(sizeof(struct tensor_2d));
	out->r = A->r;
	out->c = B->c;
	out->data = malloc(sizeof(float) * out->r * out->c);
    Bt = transpose_tensor(B);
    if(!Bt) {
        printf("ENOMEM: for transpose tensor\n");
        exit(1);
    }

// Macbook pro m2. This might be different on your machine.
    int l1d = 65536;
    int l2 = 1398101; // per cpu

    int C = l1d + l2; // cache capacity
    int Ce = sizeof(float) * (A->c + Bt->c); // Cache usage per element in out[][].

    int E = C / Ce; // Elements allowable in tile grid.

    if(E < TILE_THRESHOLD) { // when A and Bt get horizontally long, we can have a very small R value.. Which will mess up TILE_ROWS.
                                // So, we define a threshold for this.
        E = TILE_THRESHOLD;
    }
    
    TILE_COLS = 32; // m2 pro cache line size = 128 bytes = 32 floats.
    TILE_ROWS = E / TILE_COLS;

    if(TILE_ROWS > TILE_COLS)  { // we would prefer to have a row major dominant tile for better cache usage.
        int temp = TILE_ROWS;
        TILE_ROWS = TILE_COLS;
        TILE_COLS = temp;
    }

}
/*
    So, macbook pro m2 has L1d cache: 64KB L2: 4MB shared by 3 cpus (there are 10), so 1.33MB per CPU.

    So, tile_rows * tile_cols will give us number of output elements in each tile.
    Each output element will demand a corresponding row and corresponding column from A and B.

    So, since B is trasnposed, ill call it Bt. out[i][j] = sum(A[i][k] * Bt[j][k]);
    So each output element will need, A[i] and Bt[j] in cache for best perf.
    So, A[i]'s size if A->c and same with Bt, Bt->c.

    So, we will need enough cache to fit (tile_rows * tile_cols) * (A->c + Bt->c) * sizeof(float). Find your machine's cache size and
    reset TILE_ROWS and TILE_COLS. Check precompute(). BTW, I have a feeling that performace will be a function of the
    cache line as well, but here lets ignore that for now.

*/

/*
    If you notice, for(ti) and for(tj) can drop some rows and columns
    of the matrix if Ar and Bc are not multiples of TILE_ROWS and TILE_COLS.
    This in a real world scenario would be handled by padding the matrices with
    these extra rows and cols to avoid this. Here i am going to assume
    they are multiples for simplicity.
*/

void compute() {



    for(int i = 0; i < A->r; i += TILE_ROWS) {
        for(int j = 0; j < Bt->r; j += TILE_COLS) {
            for(int ti = i; ti < i + TILE_ROWS; ti++) {
                for(int tj = j; tj < j + TILE_COLS; tj++) {
                    for(int k = 0; k < A->c; k++) {
                        out->data[ti * out->c + tj] += A->data[ti * A->c + k] * Bt->data[tj*Bt->c + k];
                    }
                }
            }
        }
    }

}

int main() {
	
	input();

	precompute();
	
	time_begin = hpc_now();
	compute();
	time_end = hpc_now();

	output();
}
