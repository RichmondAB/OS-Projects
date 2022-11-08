int *mtxMul (int *c, int *a, int *b, int n) {

#pragma omp parallel for collapse(2)
  for (int i = 0; i < n; i++) {

    for (int j = 0; j < n; j++) {

      *(c + j*n + i) = 0;
      for (int k = 0; k < n; k++) {

	(*(c + j*n + i)) = (*(c + j*n + i)) +(*(a + j*n + k)) * (*(b + k*n + i));

      }

    }

  }

  return c;
  
}
