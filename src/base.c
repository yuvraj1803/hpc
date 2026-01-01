#include "timer.h"
#include <stdio.h>

double time_begin;
double time_end;

void input() {

}

void output() {
	printf("time: %f\n", (time_end - time_begin));
}

void precompute() {

}

void compute() {

}

int main() {
	
	input();

	precompute();
	
	time_begin = hpc_now();
	compute();
	time_end = hpc_now();

	output();
}
