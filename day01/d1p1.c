#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

int main() {
  int previous;
  int current;
  int result = 0;

#ifdef BENCH
  clock_t start = clock();
#endif

  if (1 == fscanf(stdin, "%d", &previous)) {
    while (fscanf(stdin, "%d", &current) > 0) {
      if (current > previous) {
        result++;
      }

      previous = current;
    }
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", result);
  return 0;
}
