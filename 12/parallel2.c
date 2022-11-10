#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>

long fib (int n) {return (n < 2 ? 1 : fib (n - 1) + fib (n - 2)); }

int main (int argc, char** argv) {
	
	int max = atoi(argv[1]);
	clock_t t = clock();
	#pragma omp parallel for 
		for (int n = 1; n <= max; n++)
			printf ("%d: %d %ld\n", omp_get_thread_num(), n, fib (n));
	t = clock() - t;
	Sleep(2);
	printf("parallel2 took %.4f seconds to execute \n", ((double) t)/CLOCKS_PER_SEC);
	return 0;
	
}