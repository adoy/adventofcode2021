#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#define MISSED INT_MIN

typedef struct {
  int minX;
  int maxX;
  int minY;
  int maxY;
} target_t;

int testProbe(target_t *t, int vx, int vy) {
  int cx = 0, cy = 0, mh = 0;
  while (cx <= t->maxX && cy >= t->minY) {
    cx += vx;
    cy += vy;
    if (vx)
      vx += vx > 0 ? -1 : 1;
    vy--;
    if (cy > mh) {
      mh = cy;
    }
    if (cx >= t->minX && cx <= t->maxX && cy >= t->minY && cy <= t->maxY) {
      return mh;
    }
  }

  return MISSED;
}

int calcMinX(target_t *t) {
  int dist = t->minX;
  int i;
  for (i = 1; i < dist; i++) {
    int vx = i;
    int x = 0;
    while (vx >= 0) {
      x += vx--;
      if (x >= dist) {
        return i;
      }
    }
  }

  exit(1);
}

int main() {
  target_t t;
  int x, y, result1 = 0, result2 = 0, startX;

#ifdef BENCH
  clock_t start = clock();
#endif

  fscanf(stdin, "target area: x=%d..%d, y=%d..%d", &t.minX, &t.maxX, &t.minY,
         &t.maxY);

  startX = calcMinX(&t);

  for (y = t.minY; y <= 1000; y++) {
    for (x = startX; x <= t.maxX; x++) {
      int mh = testProbe(&t, x, y);
      if (MISSED != mh) {
        if (mh > result1)
          result1 = mh;
        result2++;
      }
    }
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result1: %d\nResult2: %d\n", result1, result2);

  return 0;
}
