#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

int main() {
  int measures[4];
  int previous = 0;
  int current = 0;
  int i = 0;
  int result = 0;

#ifdef BENCH
  clock_t start = clock();
#endif

  for (i = 0; i < 3 && 1 == fscanf(stdin, "%d", &measures[i]); i++) {
    previous += measures[i];
  }

  while (fscanf(stdin, "%d", &measures[i]) > 0) {
    current = measures[i] + measures[(i + 3) % 4] + measures[(i + 2) % 4];
    if (current > previous) {
      result++;
    }
    i = (i + 1) % 4;
    previous = current;
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", result);

  return 0;
}
