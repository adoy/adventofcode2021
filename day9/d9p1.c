#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_ROWS 100

#define INT_OFFSET 48
#define CVAL(x, y) (((x) < width && (x) >= 0 && (y) >= 0 && (y) < height) ? grid[(y)*width + (x)] : '9')

#define U_CVAL(x, y) CVAL(x, y - 1)
#define D_CVAL(x, y) CVAL(x, y + 1)
#define L_CVAL(x, y) CVAL(x - 1, y)
#define R_CVAL(x, y) CVAL(x + 1, y)

int main() {
  char grid[MAX_LINES * MAX_ROWS + 2];
  int width = 0, height = 0;
  int x, y, result = 0;
  char *line = grid;

  while (height <= MAX_LINES && fgets(line, MAX_ROWS + 2, stdin)) {
    if (!width) {
      width = strlen(line) - 1;
    }
    line += width;
    height++;
  }

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      if (CVAL(x, y) < U_CVAL(x, y) && CVAL(x, y) < R_CVAL(x, y) &&
          CVAL(x, y) < L_CVAL(x, y) && CVAL(x, y) < D_CVAL(x, y)) {
        result += CVAL(x, y) - INT_OFFSET + 1;
      }
    }
  }

  printf("Result: %d\n", result);

  return 0;
}
