#include "timer.h"
#include <stdio.h>

int main() {
	
	double time_begin = hpc_now();

	
	double time_end = hpc_now();
	printf("time: %f\n", (time_end - time_begin));

}
