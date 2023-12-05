void selection_sort(int *x, int n) {
  int i, j, min;
    
  for (i = 1 ; i <= n - 1 ; i++) {
   min = i - 1;
   
   for (j = i ; j <= n - 1 ; j++)
     if (x[j] < x[min])
       min = j;
   
     swap(&x[i-1], &x[min]);
  }
}
