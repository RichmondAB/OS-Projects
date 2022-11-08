#include "matrix2.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

void main(int argc, char ** argv) {

  int *a = malloc(sizeof(int) * 16);
  int *b = malloc(sizeof(int) * 16);
  int *c = malloc(sizeof(int) * 16);
 
  for (int l = 0; l < 4; l++) {

    for (int k = 0; k < 4; k++) {

      *(a + l*4 + k) = l;
      *(b + l*4 + k) = k;
      *(c + l*4 + k) = 0;

    }

  }
  
  struct timeval begin, end;
  gettimeofday(&begin, 0);
  
  c = mtxMul((int *) c, (int *) a, (int *) b, 4);
  
   gettimeofday(&end, 0);
   long seconds = end.tv_sec - begin.tv_sec;
   long microseconds = end.tv_usec - begin.tv_usec;
   double elapsed = seconds + microseconds*1e-6;
  
  printf("mtxmul took %.10f picoseconds to execute \n", elapsed* 1e12);
 
  for (int i = 0; i < 4; i++) {

    for(int j = 0; j < 4; j++) {

      printf("%d\t", *(c + i*4 + j));

    }

    printf("\n");
    
  }
  
}
