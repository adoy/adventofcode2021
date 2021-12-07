#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1000

static int cmp(const void *a, const void *b) { return *(int *)a - *(int *)b; }

int main() {
  int positions[BUFFER_SIZE];
  int count = 0;
  int i, med;
  int result = 0;

  while (fscanf(stdin, "%d,", &positions[count]) > 0) {
    count++;
  }

  qsort(positions, count, sizeof(int), cmp);
  if (count & 1) {
    med = positions[count / 2];
  } else {
    med = (positions[count / 2 - 1] + positions[count / 2]) / 2;
  }

  for (i = 0; i < count; i++) {
    result += abs(med - positions[i]);
  }

  printf("Result: %d\n", result);

  return 0;
}
