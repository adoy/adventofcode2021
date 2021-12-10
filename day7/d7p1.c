#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#define BUFFER_SIZE 1000

static int cmp(const void *a, const void *b) { return *(int *)a - *(int *)b; }

int main() {
  int positions[BUFFER_SIZE];
  int count = 0;
  int i, med;
  int result = 0;

#ifdef BENCH
  clock_t start = clock();
#endif

  while (fscanf(stdin, "%d,", &positions[count]) > 0) {
    count++;
  }

  qsort(positions, count, sizeof(int), cmp);
  if (count & 1) {
    med = positions[count / 2];
  } else {
    med = (positions[count / 2 - 1] + positions[count / 2]) / 2;
  }

  for (i = 0; i < count; i++) {
    result += abs(med - positions[i]);
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", result);

  return 0;
}
