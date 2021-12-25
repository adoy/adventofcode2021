#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

int magicNumbers[] = {7, 15, 2, -3, 14, -9, 15, -7, -11, -4, 12, 2, -8, -10};

#define DIGITS 14
#define searchDesc(in) search(in, 0, 0, 1)
#define searchAsc(in) search(in, 0, 0, 0)

int search(char *input, int i, int z, int desc) {
  int w;
  if (i == DIGITS && z == 0)
    return 1;

  if (magicNumbers[i] > 0) {
    for (w = 9; w > 0; w--) {
      if (search(input, i + 1, 26 * z + (desc ? w : 10 - w) + magicNumbers[i], desc)) {
        input[i] = (desc ? w : 10 - w) + '0';
        return 1;
      }
    }
  } else {
    w = (z % 26 + magicNumbers[i]);
    if (w >= 1 && w <= 9 && search(input, i + 1, z /= 26, desc)) {
      input[i] = w + '0';
      return 1;
    }
  }

  return 0;
}

int main() {
  char largest[DIGITS + 1] = {0}, smallest[DIGITS + 1] = {0};
#ifdef BENCH
  clock_t start = clock();
#endif

  searchDesc(largest);
  searchAsc(smallest);

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result1: %s\nResult2: %s\n", largest, smallest);

  return 0;
}
