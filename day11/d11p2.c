#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define LINES 10
#define ROWS 10

#define val(grid, x, y) (grid->grid[(y)*grid->width + (x)])
#define outbound(grid, x, y) ((x) < 0 || (x) >= grid->width || (y) < 0 || (y) >= grid->height)

typedef struct grid {
  int grid[LINES * ROWS];
  int width;
  int height;
} grid_t;

static int increaseOctoLevel(grid_t *grid, int x, int y) {
  int flashes = 0;

  if (outbound(grid, x, y) || val(grid, x, y) > 9) {
    return flashes;
  }

  if (++grid->grid[y * grid->width + x] > 9) {
    flashes++;
    flashes += increaseOctoLevel(grid, x - 1, y - 1);
    flashes += increaseOctoLevel(grid, x, y - 1);
    flashes += increaseOctoLevel(grid, x + 1, y - 1);
    flashes += increaseOctoLevel(grid, x - 1, y);
    flashes += increaseOctoLevel(grid, x + 1, y);
    flashes += increaseOctoLevel(grid, x - 1, y + 1);
    flashes += increaseOctoLevel(grid, x, y + 1);
    flashes += increaseOctoLevel(grid, x + 1, y + 1);
  }

  return flashes;
}

static int step(grid_t *grid) {
  int x, y, flashes = 0;
  for (y = 0; y < grid->height; y++) {
    for (x = 0; x < grid->width; x++) {
      flashes += increaseOctoLevel(grid, x, y);
    }
  }
  for (y = 0; y < grid->height; y++) {
    for (x = 0; x < grid->width; x++) {
      if (val(grid, x, y) > 9) {
        val(grid, x, y) = 0;
      }
    }
  }
  return flashes;
}

static void gridInit(grid_t *grid, FILE *fd) {
  char str[ROWS + 2];
  grid->width = 0;
  grid->height = 0;
  int i = 0;

  while (grid->height <= LINES && fgets(str, ROWS + 2, fd)) {
    if (!grid->width) {
      grid->width = strlen(str) - 1;
    }
    for (i = 0; i < grid->width; i++) {
      grid->grid[grid->height * grid->width + i] = str[i] - '0';
    }
    grid->height++;
  }
}

static int isSynchronized(grid_t *grid) {
  int i;
  for (i = grid->width * grid->height - 1; i > 0; i--) {
    if (grid->grid[0] != grid->grid[i]) {
      return 0;
    }
  }

  return 1;
}

int main() {
  grid_t grid;
  int result = 0;

#ifdef BENCH
  clock_t start = clock();
#endif

  gridInit(&grid, stdin);
  while (!isSynchronized(&grid)) {
    step(&grid);
    result++;
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", result++);

  return 0;
}
