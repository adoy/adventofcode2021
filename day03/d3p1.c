#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define INITIAL_BITCOUNTER_BUFFER_SIZE 13
#define INITIAL_LINE_BUFFER_SIZE INITIAL_BITCOUNTER_BUFFER_SIZE + 2

int main() {
  int *bitCounter;
  char *line;

  size_t bufferSize = INITIAL_BITCOUNTER_BUFFER_SIZE;
  unsigned int bitCount = 0;
  unsigned int i = 0;
  char *c = NULL;

  int gamma = 0;
  int epsilon = 0;

#ifdef BENCH
  clock_t start = clock();
#endif

  line = (char *)malloc(sizeof(char) * INITIAL_LINE_BUFFER_SIZE);

  bitCounter = (int *)malloc(sizeof(int) * bufferSize);

  while ((NULL == c || *c != '\n') && fgets(line, INITIAL_LINE_BUFFER_SIZE, stdin)) {
    c = line;
    while (*c) {
      if (bitCount >= bufferSize) {
        bufferSize *= 2;
        bitCounter = (int *)realloc(bitCounter, sizeof(int) * bufferSize);
      }
      if (*c == '1') {
        bitCounter[bitCount++] = 1;
      } else if (*c == '0') {
        bitCounter[bitCount++] = -1;
      } else if (*c == '\n') {
        break;
      }
      c++;
    };
  }

  line = (char *)realloc(line, sizeof(char) * (bitCount + 2));

  while (fgets(line, bitCount + 2, stdin)) {
    for (i = 0; i < bitCount; i++) {
      bitCounter[i] += (line[i] == '0') ? -1 : 1;
    }
  }

  for (i = 0; i < bitCount; i++) {
    *((bitCounter[i] > 1) ? &gamma : &epsilon) |= 1 << (bitCount - 1 - i);
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Gamma: %d\nEpsilon: %d\n", gamma, epsilon);
  printf("Result: %d\n", epsilon * gamma);

  free(bitCounter);
  free(line);

  return 0;
}
