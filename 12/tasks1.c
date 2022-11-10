#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

int main(int argc, char** argv) {

	int max = atoi(argv[1]);
	int tasks = atoi(argv[2]);
	if (max % tasks != 0) return 1;
	int sum = 0;
	clock_t t = clock();
	#pragma omp parallel
	{
	
		#pragma omp single
		for (int t = 0; t < tasks; t++) {
		
			#pragma omp task
			{
			
				int local_sum = 0;
				int lo = (max / tasks) * (t + 0) + 1;
				int hi = (max / tasks) * (t + 1) + 0;
				printf("%d: %d\t%d\n", omp_get_thread_num(), lo, hi);
				for (int i = lo; i <= hi; i++)
					local_sum = local_sum + 1;
				#pragma omp atomic
					sum = sum + local_sum;
				
			
			}
		
		}
	
	}
		t = clock() - t;
  
  Sleep(2);
  printf("tasks1 took %.4f seconds to execute \n", ((double) t)/CLOCKS_PER_SEC);
	printf("%d\n", sum);
	
}