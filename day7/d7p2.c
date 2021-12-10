#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#define BUFFER_SIZE 1000

int main() {
  int positions[BUFFER_SIZE];
  int count = 0;
  int i, x, avg = 0;
  int result1 = 0;
  int result2 = 0;

#ifdef BENCH
  clock_t start = clock();
#endif

  while (fscanf(stdin, "%d,", &positions[count]) > 0) {
    avg += positions[count];
    count++;
  }

  avg = avg / count;

  for (i = 0; i < count; i++) {
    x = abs(avg - positions[i]);
    result1 += (x * (x + 1)) / 2;
    x = abs(avg + 1 - positions[i]);
    result2 += (x * (x + 1)) / 2;
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", result1 < result2 ? result1 : result2);

  return 0;
}
