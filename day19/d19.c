#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#define LINE_BUFFER 100
#define MAX_POINTS 30
#define MAX_SCANNERS 40
#define UNKNOWN_ORIENTATION -1

typedef struct pt { int x, y, z; } pt_t;

typedef struct pts {
  pt_t points[MAX_POINTS];
  int count;
} pts_t;

typedef struct scanner {
  int orientation;
  pt_t position;
  pts_t points;
} scanner_t;

typedef struct sc {
  scanner_t scanners[MAX_SCANNERS];
  int count;
} sc_t;

static pt_t pt_orient(pt_t p, int o) {
  if (o == 0) return (pt_t){p.x, p.y, p.z};
  else if (o == 1) return (pt_t){p.x, -p.z, p.y};
  else if (o == 2) return (pt_t){p.x, -p.y, -p.z};
  else if (o == 3) return (pt_t){p.x, p.z, -p.y};
  else if (o == 4) return (pt_t){-p.x, p.y, -p.z};
  else if (o == 5) return (pt_t){-p.x, p.z, p.y};
  else if (o == 6) return (pt_t){-p.x, -p.y, p.z};
  else if (o == 7) return (pt_t){-p.x, -p.z, -p.y};
  else if (o == 8) return (pt_t){p.y, p.z, p.x};
  else if (o == 9) return (pt_t){p.y, -p.x, p.z};
  else if (o == 10) return (pt_t){p.y, -p.z, -p.x};
  else if (o == 11) return (pt_t){p.y, p.x, -p.z};
  else if (o == 12) return (pt_t){-p.y, p.z, -p.x};
  else if (o == 13) return (pt_t){-p.y, p.x, p.z};
  else if (o == 14) return (pt_t){-p.y, -p.z, p.x};
  else if (o == 15) return (pt_t){-p.y, -p.x, -p.z};
  else if (o == 16) return (pt_t){p.z, p.x, p.y};
  else if (o == 17) return (pt_t){p.z, -p.y, p.x};
  else if (o == 18) return (pt_t){p.z, -p.x, -p.y};
  else if (o == 19) return (pt_t){p.z, p.y, -p.x};
  else if (o == 20) return (pt_t){-p.z, p.x, -p.y};
  else if (o == 21) return (pt_t){-p.z, p.y, p.x};
  else if (o == 22) return (pt_t){-p.z, -p.x, p.y};
  else return (pt_t){-p.z, -p.y, -p.x};
}

static pts_t pts_orient(pts_t *src, int o) {
  int i;
  pts_t pts;
  pts.count = src->count;
  for (i = 0; i < pts.count; i++) {
    pts.points[i] = pt_orient(src->points[i], o);
  }
  return pts;
}

static pt_t pt_add(pt_t *p1, pt_t *p2) {
  return (pt_t){p1->x + p2->x, p1->y + p2->y, p1->z + p2->z};
}

static pts_t pts_add(pts_t *src, pt_t *p) {
  int i;
  pts_t pts;
  pts.count = src->count;
  for (i = 0; i < pts.count; i++) {
    pts.points[i] = pt_add(&src->points[i], p);
  }
  return pts;
}

static pt_t pt_sub(pt_t *p1, pt_t *p2) {
  return (pt_t){p1->x - p2->x, p1->y - p2->y, p1->z - p2->z};
}

static int pt_equals(pt_t *p1, pt_t *p2) {
  return (p1->x == p2->x) && (p1->y == p2->y) && (p1->z == p2->z);
}

static int superimpose(pts_t *rpts, pts_t *pts) {
  int i, j, c = 0;
  for (i = 0; i < rpts->count; i++) {
    for (j = 0; j < pts->count; j++) {
      if (pt_equals(&rpts->points[i], &pts->points[j])) {
        if (++c >= 12) {
          return 1;
        }
      }
    }
  }
  return 0;
}

static int tryLocateScanner(scanner_t *s, scanner_t *ref) {
  int i, o, j;
  pts_t rpts = pts_orient(&ref->points, ref->orientation);

  for (i = 0; i < rpts.count; i++) {
    for (o = 0; o < 24; o++) {
      pts_t pts = pts_orient(&s->points, o);
      for (j = 0; j < pts.count; j++) {
        pt_t dist = pt_sub(&rpts.points[i], &pts.points[j]);
        pts_t pts2 = pts_add(&pts, &dist);
        if (superimpose(&rpts, &pts2)) {
          s->orientation = o;
          s->position = pt_add(&dist, &ref->position);
          return 1;
        }
      }
    }
  }
  return 0;
}

void loadScannerReports(FILE *fp, sc_t *sc) {
  char line[LINE_BUFFER];
  while (fgets(line, LINE_BUFFER, fp)) {
    sc->scanners[sc->count].orientation =
        (0 == sc->count) ? 0 : UNKNOWN_ORIENTATION;
    sc->scanners[sc->count].position = (pt_t){0, 0, 0};
    sc->scanners[sc->count].points.count = 0;
    while (fgets(line, LINE_BUFFER, fp)) {
      if ('\n' == line[0])
        break;
      sscanf(line, "%d,%d,%d",
             &sc->scanners[sc->count]
                  .points.points[sc->scanners[sc->count].points.count]
                  .x,
             &sc->scanners[sc->count]
                  .points.points[sc->scanners[sc->count].points.count]
                  .y,
             &sc->scanners[sc->count]
                  .points.points[sc->scanners[sc->count].points.count]
                  .z);
      sc->scanners[sc->count].points.count++;
    }
    sc->count++;
  }
}

void locateScanners(sc_t *sc) {
  int locatedCount = 1, i, j;
  while (locatedCount != sc->count) {
    for (i = 0; i < sc->count; i++) {
      if (UNKNOWN_ORIENTATION != sc->scanners[i].orientation) {
        continue;
      }
      for (j = 0; j < sc->count; j++) {
        if (UNKNOWN_ORIENTATION != sc->scanners[j].orientation) {
          if (tryLocateScanner(&sc->scanners[i], &sc->scanners[j])) {
            locatedCount++;
            break;
          }
        }
      }
    }
  }
}

int countBeacons(sc_t *sc) {
  int i, j, pc = 0, c = 0;
  pt_t points[MAX_SCANNERS * MAX_POINTS];
  for (i = 0; i < sc->count; i++) {
    pts_t pts = pts_orient(&sc->scanners[i].points, sc->scanners[i].orientation);
    pts = pts_add(&pts, &sc->scanners[i].position);
    for (j = 0; j < pts.count; j++) {
      points[pc++] = pts.points[j];
    }
  }
  for (i = 0; i < pc; i++) {
    for (j = i + 1; j < pc; j++) {
      if (pt_equals(&points[i], &points[j]))
        break;
    }
    if (j == pc)
      c++;
  }
  return c;
}

int largestManhattanDistance(sc_t *sc) {
  int i, j, md, lmd = 0;
  for (i = 0; i < sc->count; i++) {
    for (j = i + 1; j < sc->count; j++) {
      pt_t p = pt_sub(&sc->scanners[i].position, &sc->scanners[j].position);
      md = abs(p.x) + abs(p.y) + abs(p.z);
      if (md > lmd)
        lmd = md;
    }
  }
  return lmd;
}

int main() {
  int result1 = 0, result2 = 0;
  sc_t sc = {0};

#ifdef BENCH
  clock_t start = clock();
#endif

  loadScannerReports(stdin, &sc);
  locateScanners(&sc);
  result1 = countBeacons(&sc);
  result2 = largestManhattanDistance(&sc);

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result1: %d\nResult2: %d\n", result1, result2);

  return 0;
}
