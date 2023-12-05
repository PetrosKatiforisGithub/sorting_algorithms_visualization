void insertion_sort(int *x, int n) {
  int i, j;
  for (i = 1 ; i <= n - 1 ; i++) {
    j = i - 1;
    while (j >= 0 && x[j] > x[j+1]) {
      swap(&x[j], &x[j+1]);
      j--;
    }
  }  
}
