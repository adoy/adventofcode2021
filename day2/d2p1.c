#include <stdio.h>
#include <stdlib.h>

int main() {
  char direction[8];
  int velocity = 0;

  int horizontalPos = 0;
  int depth = 0;

  while (2 == fscanf(stdin, "%7s %d", direction, &velocity)) {
    if ('f' == direction[0]) {
      horizontalPos += velocity;
    } else if ('d' == direction[0]) {
      depth += velocity;
    } else if ('u' == direction[0]) {
      depth -= velocity;
    }
  }

  printf("Result: %d\n", horizontalPos * depth);
  return 0;
}
