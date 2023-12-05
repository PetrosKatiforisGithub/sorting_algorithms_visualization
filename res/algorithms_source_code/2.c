void bubblesort(int *x, int n) {
  int i, j;
  for (i = 1; i <= n - 1; i++)
    for (j = n - 1; j >= i; j--)
      if (x[j-1] > x[j])
        swap(&x[j-1], &x[j]);
}
