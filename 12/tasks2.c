#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

long fib (int n) {return (n < 2 ? 1 : fib (n - 1) + fib (n - 2)); }

int main (int argc, char** argv) {
	
	int max = atoi(argv[1]);
	#pragma omp parallel
		#pragma omp single
			for (int n = 1; n <= max; n++)
				#pragma omp task
					printf ("%d: %d %ld\n", omp_get_thread_num(), n, fib (n));
	return 0;
	
}