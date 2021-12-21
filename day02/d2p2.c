#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

int main() {
  char direction[8];
  int velocity = 0;

  int horizontalPos = 0;
  int depth = 0;
  int aim = 0;

#ifdef BENCH
  clock_t start = clock();
#endif

  while (2 == fscanf(stdin, "%7s %d", direction, &velocity)) {
    if ('f' == direction[0]) {
      horizontalPos += velocity;
      depth += (aim * velocity);
    } else if ('d' == direction[0]) {
      aim += velocity;
    } else if ('u' == direction[0]) {
      aim -= velocity;
    }
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", horizontalPos * depth);
  return 0;
}
