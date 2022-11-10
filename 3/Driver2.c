#include "matrix2.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

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
  
  clock_t t = clock();
  printf("%d\n", t);
  
  c = mtxMul((int *) c, (int *) a, (int *) b, 4);
  printf("%d\n", clock());
  t = clock() - t;
  
  Sleep(2);
  printf("mtxmul took %.10f seconds to execute \n", ((double) t)/CLOCKS_PER_SEC);
 
  for (int i = 0; i < 4; i++) {

    for(int j = 0; j < 4; j++) {

      printf("%d\t", *(c + i*4 + j));

    }

    printf("\n");
    
  }
  
}
