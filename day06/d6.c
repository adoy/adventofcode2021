#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define MAX_AGE 9

int main(int argc, char **argv) {
  unsigned long today[MAX_AGE] = {0L};
  unsigned long tomorrow[MAX_AGE];
  unsigned long result = 0;
  int i, day = 0;

#ifdef BENCH
  clock_t start = clock();
  day = (argc < 2) ? 250 : atoi(argv[1]);
#else
  if (argc < 2) {
    fprintf(stderr, "Usage: %s NUMBEROFDAYS < input.txt\n", argv[0]);
    return 1;
  }
  day = atoi(argv[1]);
#endif


  while (fscanf(stdin, "%d,", &i) > 0) today[i]++;

  while (day--) {
    for (i = MAX_AGE - 1; i >= 0; i--) {
      tomorrow[i] = today[(i + 1) % MAX_AGE];
    }
    tomorrow[6] += today[0];
    memcpy(today, tomorrow, sizeof(tomorrow));
  }

  for (i = 0; i < MAX_AGE; i++) result += today[i];

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %ld\n", result);

  return 0;
}
