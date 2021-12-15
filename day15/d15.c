#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define MAX_LINES 100
#define MAX_ROWS 100

#define IS_INBOUND(g, x, y)                                                    \
  ((x) < (g)->width && (x) >= 0 && (y) >= 0 && (y) < g->height)
#define VAL(g, x, y) ((g)->grid[(y) * (g)->width + (x)])
#define SET_VISITED(g, x, y) (VAL(g, x, y) = -1)
#define IS_VISITED(g, x, y) (VAL(g, x, y) == -1)

typedef struct {
  int8_t *grid;
  int width, height;
} grid_t;

typedef struct pq {
  int x, y, risk;
  struct pq *next;
} pq_t;

void freePq(pq_t *pq) {
  pq_t *next;
  while (pq) {
    next = pq->next;
    free(pq);
    pq = next;
  }
}

pq_t *pqPop(pq_t **pq) {
  pq_t *head = *pq;
  if (head) {
    *pq = head->next;
  }
  return head;
}

void pqPush(pq_t **pq, int x, int y, int risk) {
  pq_t *new = (pq_t *)malloc(sizeof(pq_t));
  new->x = x;
  new->y = y;
  new->risk = risk;

  if (NULL == *pq || risk <= (*pq)->risk) {
    new->next = *pq;
    *pq = new;
  } else {
    pq_t *current = *pq;
    while (NULL != current->next && risk > current->next->risk) {
      current = current->next;
    }
    new->next = current->next;
    current->next = new;
  }
}

grid_t *gridInit(FILE *fp) {
  char line[MAX_LINES + 2];
  int i;
  grid_t *grid = (grid_t *)malloc(sizeof(grid_t));
  grid->grid = (int8_t *)malloc(sizeof(int8_t) * MAX_LINES * MAX_ROWS);

  grid->width = grid->height = 0;
  while (grid->height <= MAX_LINES && fgets(line, MAX_ROWS + 2, fp)) {
    if (!grid->width) {
      grid->width = strlen(line) - 1;
    }
    for (i = 0; i < grid->width; i++) {
      grid->grid[grid->height * grid->width + i] = line[i] - '0';
    }
    grid->height++;
  }

  return grid;
}

void freeGrid(grid_t *grid) {
  free(grid->grid);
  free(grid);
}

grid_t *gridMultiply(grid_t *grid, int factor) {
  int x, y;
  grid_t *new = (grid_t *)malloc(sizeof(grid_t));
  new->width = grid->width *factor;
  new->height = grid->height *factor;
  new->grid = (int8_t *)malloc(sizeof(int8_t) * new->width * new->height);

  for (y = 0; y < new->height; y++) {
    for (x = 0; x < new->width; x++) {
      int8_t v = VAL(grid, y % grid->height, x % grid->width) + (x / grid->width) + (y / grid->height);
      new->grid[y * new->width + x] = v > 9 ? v % 9 : v;
    }
  }

  return new;
}

int calculateRisk(grid_t *grid) {
  int x, y, risk = 0;
  pq_t *current, *pq = NULL;
  pqPush(&pq, 0, 0, 0);

  while (pq) {
    current = pqPop(&pq);
    x = current->x;
    y = current->y;
    risk = current->risk;
    free(current);

    if (IS_VISITED(grid, x, y)) {
      continue;
    }
    SET_VISITED(grid, x, y);

    if (x == grid->width - 1 && y == grid->height - 1) {
      freePq(pq);
      return risk;
    }

    if (IS_INBOUND(grid, x - 1, y))
      pqPush(&pq, x - 1, y, risk + VAL(grid, x - 1, y));
    if (IS_INBOUND(grid, x + 1, y))
      pqPush(&pq, x + 1, y, risk + VAL(grid, x + 1, y));
    if (IS_INBOUND(grid, x, y - 1))
      pqPush(&pq, x, y - 1, risk + VAL(grid, x, y - 1));
    if (IS_INBOUND(grid, x, y + 1))
      pqPush(&pq, x, y + 1, risk + VAL(grid, x, y + 1));
  }

  freePq(pq);
  exit(1);
}

int main() {
  grid_t *grid1, *grid2;
  int risk1, risk2;

#ifdef BENCH
  clock_t start = clock();
#endif

  grid1 = gridInit(stdin);
  grid2 = gridMultiply(grid1, 5);

  risk1 = calculateRisk(grid1);
  risk2 = calculateRisk(grid2);

  freeGrid(grid1);
  freeGrid(grid2);

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result1: %d\nResult2: %d\n", risk1, risk2);

  return 0;
}
