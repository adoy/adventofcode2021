#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define LINE_BUFFER 102
#define IEA_BUFFER 1000

#define getChar(g,x,y,d) (((x) < 0 || (y) < 0 || (x) >= (g)->width || (y) >= (g)->height) ? (d) : (g)->grid[(y) * (g)->width + (x)])

typedef struct grid {
  char *grid;
  int size;
  int width;
  int height;
} grid_t;

static grid_t *gridInit(size_t s) {
  grid_t *g = (grid_t *)malloc(sizeof(grid_t));
  g->width = 0;
  g->height = 0;
  g->size = s;
  g->grid = (char *)malloc(sizeof(char) * s);
  return g;
}

static void gridFree(grid_t *g) {
  free(g->grid);
  free(g);
}

static int getIEAIndex(grid_t *g, int x, int y, char d) {
  int i, j, r = 0;
  for (j = -1; j <= 1; j++) {
    for (i = -1; i <= 1; i++) {
      r = (r << 1) | ('#' == getChar(g, x + i, y + j, d) ? 1 : 0);
    }
  }
  return r;
}

static char gridEnhance(grid_t **g, char *iea, char d) {
  int x, y;

  grid_t *tmp = *g;
  grid_t *ng = gridInit(((*g)->width + 2) * ((*g)->height + 2));
  ng->width = (*g)->width + 2;
  ng->height = (*g)->height + 2;

  for (y = -1; y < (*g)->height + 1; y++) {
    for (x = -1; x < (*g)->width + 1; x++) {
      ng->grid[(y + 1) * ng->width + (x + 1)] = iea[getIEAIndex(*g, x, y, d)];
    }
  }

  (*g) = ng;
  gridFree(tmp);

  return iea[(d == '#' ? 1 : 0) * ((1 << 9) - 1)];
}

static int gridCount(grid_t *g) {
  int x, y, c = 0;
  for (y = 0; y < g->height; y++) {
    for (x = 0; x < g->width; x++) {
      if ('#' == getChar(g, x, y, '.')) {
        c++;
      }
    }
  }
  return c;
}

static void loadImage(grid_t *g) {
  char *s = g->grid;
  fgets(s, LINE_BUFFER, stdin);
  while (fgets(s, LINE_BUFFER, stdin)) {
    if (!g->width) {
      g->width = strlen(s) - 1;
    }
    s += g->width;
    g->height++;
  }
}

int main() {
  char iea[IEA_BUFFER];
  grid_t *g = gridInit(100 * 100);
  int i, result1, result2;
  char d = '.';

#ifdef BENCH
  clock_t start = clock();
#endif

  fgets(iea, IEA_BUFFER, stdin);
  loadImage(g);
  for (i = 0; i < 2; i++) {
    d = gridEnhance(&g, iea, d);
  }
  result1 = gridCount(g);
  for ( ; i < 50; i++) {
    d = gridEnhance(&g, iea, d);
  }
  result2 = gridCount(g);
  gridFree(g);

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result1: %d\nResult2: %d\n", result1, result2);

  return 0;
}
