#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define MAX_LINES 1000
#define MAX_ROWS 100

#define CVAL(x, y) (((x) < width && (x) >= 0 && (y) >= 0 && (y) < height) ? grid[(y)*width + (x)] : '9')

#define U_CVAL(x, y) CVAL(x, y - 1)
#define D_CVAL(x, y) CVAL(x, y + 1)
#define L_CVAL(x, y) CVAL(x - 1, y)
#define R_CVAL(x, y) CVAL(x + 1, y)

static int getBasinSizeEx(char *grid, int width, int height, int x, int y) {
  int size = 1;
  int depth = CVAL(x, y);
  grid[y * width + x] = '9';

  if (U_CVAL(x, y) < '9' && U_CVAL(x, y) > depth)
    size += getBasinSizeEx(grid, width, height, x, y - 1);
  if (D_CVAL(x, y) < '9' && D_CVAL(x, y) > depth)
    size += getBasinSizeEx(grid, width, height, x, y + 1);
  if (L_CVAL(x, y) < '9' && L_CVAL(x, y) > depth)
    size += getBasinSizeEx(grid, width, height, x - 1, y);
  if (R_CVAL(x, y) < '9' && R_CVAL(x, y) > depth)
    size += getBasinSizeEx(grid, width, height, x + 1, y);

  return size;
}

static int getBasinSize(char *grid, int width, int height, int x, int y) {
  char *gridCopy = (char *) malloc(sizeof(char) * width * height);
  memcpy(gridCopy, grid, width * height);
  int size = getBasinSizeEx(gridCopy, width, height, x, y);
  free(gridCopy);

  return size;
}

int main() {
  char grid[MAX_LINES * MAX_ROWS + 2];
  int width = 0, height = 0;
  int x, y, i, size = 0;
  char *line = grid;
  int biggestBassinsSize[3] = {0};

#ifdef BENCH
  clock_t start = clock();
#endif

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
        size = getBasinSize(grid, width, height, x, y);
        for (i = 0; i < 3; i++) {
          if (size > biggestBassinsSize[i]) {
            int old = biggestBassinsSize[i];
            biggestBassinsSize[i] = size;
            size = old;
          }
        }
      }
    }
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n",
         biggestBassinsSize[0] * biggestBassinsSize[1] * biggestBassinsSize[2]);

  return 0;
}
