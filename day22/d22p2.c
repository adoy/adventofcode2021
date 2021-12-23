#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#ifdef BENCH
#include <time.h>
#endif

#define MAX_CUBOIDS 420

typedef struct {
  int8_t state;
  int minX;
  int maxX;
  int minY;
  int maxY;
  int minZ;
  int maxZ;
} cuboid;

static int cmp(const void *a, const void *b) { return *(int *)a - *(int *)b; }

int loadCuboids(cuboid *c, FILE *fp) {
  int n = 0;
  char state[4];

  while (n < MAX_CUBOIDS && fscanf(fp, "%s x=%d..%d,y=%d..%d,z=%d..%d\n", state, &c[n].minX, &c[n].maxX, &c[n].minY, &c[n].maxY, &c[n].minZ, &c[n].maxZ) > 0) {
    c[n].state = (state[1] == 'n') ? 1 : 0;
    c[n].maxX++;
    c[n].maxY++;
    c[n].maxZ++;
    n++;
  }

  return n;
}

int getSplitIndex(int *L, int l) {
  int i = 0;
  do {
    if (L[i] == l)
      return i;
  } while (++i);

  return -1;
}

int main() {
  cuboid c[MAX_CUBOIDS];

  int splitX[MAX_CUBOIDS * 2] = {0};
  int splitY[MAX_CUBOIDS * 2] = {0};
  int splitZ[MAX_CUBOIDS * 2] = {0};
  int i, cc = loadCuboids(c, stdin);
  int x, y, z;
  int8_t *s;
  int splitPerAxe;
  unsigned long result = 0L;

#ifdef BENCH
  clock_t start = clock();
#endif

  for (i = 0; i < cc; i++) {
    splitX[i] = c[i].minX;
    splitY[i] = c[i].minY;
    splitZ[i] = c[i].minZ;
    splitX[i + cc] = c[i].maxX;
    splitY[i + cc] = c[i].maxY;
    splitZ[i + cc] = c[i].maxZ;
  }

  splitPerAxe = cc * 2;

  qsort(splitX, splitPerAxe, sizeof(int), cmp);
  qsort(splitY, splitPerAxe, sizeof(int), cmp);
  qsort(splitZ, splitPerAxe, sizeof(int), cmp);

  s = (int8_t *)calloc(splitPerAxe * splitPerAxe * splitPerAxe, sizeof(int8_t));

  for (i = 0; i < cc; i++) {
    int xmin = getSplitIndex(splitX, c[i].minX);
    int xmax = getSplitIndex(splitX, c[i].maxX);
    int ymin = getSplitIndex(splitY, c[i].minY);
    int ymax = getSplitIndex(splitY, c[i].maxY);
    int zmin = getSplitIndex(splitZ, c[i].minZ);
    int zmax = getSplitIndex(splitZ, c[i].maxZ);
    for (x = xmin; x < xmax; x++)
      for (y = ymin; y < ymax; y++)
        for (z = zmin; z < zmax; z++)
          s[z * splitPerAxe * splitPerAxe + y * splitPerAxe + x] = c[i].state;
  }

  for (x = 0; x < splitPerAxe - 1; x++)
    for (y = 0; y < splitPerAxe - 1; y++)
      for (z = 0; z < splitPerAxe - 1; z++)
        if (s[z * splitPerAxe * splitPerAxe + y * splitPerAxe + x])
          result += (long)(splitX[x + 1] - splitX[x]) * (splitY[y + 1] - splitY[y]) * (splitZ[z + 1] - splitZ[z]);

  free(s);

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %lu\n", result);

  return 0;
}
