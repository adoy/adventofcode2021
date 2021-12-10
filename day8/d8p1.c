#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#define LINE_BUFFER_SIZE 255

int main() {
  char line[LINE_BUFFER_SIZE];
  int i, result = 0;

#ifdef BENCH
  clock_t start = clock();
#endif

  while (fgets(line, LINE_BUFFER_SIZE, stdin)) {
    char inDigit = 0;
    int length = 0;

    for (i = 0; i < LINE_BUFFER_SIZE && '\n' != line[i] && 0 != line[i]; i++) {
      switch (line[i]) {
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
        if (inDigit) length++;
        break;
      case ' ':
        if (length == 2 || length == 4 || length == 3 || length == 7) {
          result++;
        }
        length = 0;
        break;
      case '|':
        inDigit = 1;
        i++;
        break;
      }
    }
    if (length == 2 || length == 4 || length == 3 || length == 7) {
      result++;
    }
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", result);

  return 0;
}
