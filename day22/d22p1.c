#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#ifdef BENCH
#include <time.h>
#endif

#define MAX_CUBOIDS 420
#define B 50
#define D1 (B * 2 + 1)
#define D2 (D1 * D1)
#define D3 (D1 * D1 * D1)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct { int state, minX, maxX, minY, maxY, minZ, maxZ; } cuboid;

int loadCuboids(cuboid *c, FILE *fp) {
  int n = 0;
  char state[4];
  int minX, maxX, minY, maxY, minZ, maxZ;

  while (n < MAX_CUBOIDS && fscanf(fp, "%s x=%d..%d,y=%d..%d,z=%d..%d\n", state, &minX, &maxX, &minY, &maxY, &minZ, &maxZ) > 0) {
    c[n].state = (state[1] == 'n') ? 1 : 0;
    c[n].minX = MAX(-B, minX) + B;
    c[n].minY = MAX(-B, minY) + B;
    c[n].minZ = MAX(-B, minZ) + B;
    c[n].maxX = MIN(+B, maxX) + B;
    c[n].maxY = MIN(+B, maxY) + B;
    c[n].maxZ = MIN(+B, maxZ) + B;
    n++;
  }

  return n;
}

int main() {
  cuboid c[MAX_CUBOIDS];
  int cc = loadCuboids(c, stdin);
  int result = 0;
  int x, y, z, i;
  int8_t s[D3] = {0};

#ifdef BENCH
  clock_t start = clock();
#endif

  for (i = 0; i < cc; i++)
    for (x = c[i].minX; x <= c[i].maxX; x++)
      for (y = c[i].minY; y <= c[i].maxY; y++)
        for (z = c[i].minZ; z <= c[i].maxZ; z++)
          s[z * D2 + y * D1 + x] = c[i].state;

  for (x = 0; x < D1; x++)
    for (y = 0; y < D1; y++)
      for (z = 0; z < D1; z++)
        result += s[z * D2 + y * D1 + x];

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", result);

  return 0;
}
