#include <stdio.h>
#include <stdlib.h>

int main() {
  int previous;
  int current;
  int result = 0;

  if (1 == fscanf(stdin, "%d", &previous)) {
    while (fscanf(stdin, "%d", &current) > 0) {
      if (current > previous) {
        result++;
      }

      previous = current;
    }
  }

  printf("Result: %d\n", result);
  return 0;
}
