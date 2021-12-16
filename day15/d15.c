#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define MAX_LINES 100
#define MAX_ROWS 100

#define parentIndex(i) (((i)-1) / 2)
#define leftChildIndex(i) (2 * (i) + 1)
#define rightChildIndex(i) (2 * (i) + 2)
#define swap(a, b)                                                             \
  do {                                                                         \
    struct state *tmp = a;                                                     \
    a = b;                                                                     \
    b = tmp;                                                                   \
  } while (0)

#define IS_INBOUND(g, x, y)                                                    \
  ((x) < (g)->width && (x) >= 0 && (y) >= 0 && (y) < g->height)
#define VAL(g, x, y) ((g)->grid[(y) * (g)->width + (x)])
#define SET_VISITED(g, x, y) (VAL(g, x, y) = -1)
#define IS_VISITED(g, x, y) (VAL(g, x, y) == -1)

typedef struct {
  int8_t *grid;
  int width, height;
} grid_t;

typedef struct {
  int size;
  int count;
  struct state {
    int x;
    int y;
    int risk;
  } * *data;
} minHeap;

minHeap *initHeap(size_t initialSize) {
  minHeap *heap = (minHeap *)malloc(sizeof(minHeap));
  heap->size = initialSize;
  heap->count = 0;
  heap->data = (struct state **)malloc(sizeof(struct state *) * heap->size);

  return heap;
}

void siftUp(minHeap *heap, int ci) {
  int pi = parentIndex(ci);
  while (pi >= 0 && heap->data[pi]->risk > heap->data[ci]->risk) {
    swap(heap->data[pi], heap->data[ci]);
    ci = pi;
    pi = parentIndex(ci);
  }
}

void siftDown(minHeap *heap, int pi) {
  int lci = leftChildIndex(pi);

  while (lci < heap->count) {
    int sci = lci;
    int rci = rightChildIndex(pi);
    if (rci < heap->count && heap->data[rci]->risk < heap->data[lci]->risk) {
      sci = rci;
    }
    if (heap->data[sci]->risk < heap->data[pi]->risk) {
      swap(heap->data[sci], heap->data[pi]);
      pi = sci;
      lci = leftChildIndex(pi);
    } else {
      break;
    }
  }
}

void insertHeap(minHeap *heap, int x, int y, int risk) {
  struct state *state = (struct state *)malloc(sizeof(struct state));
  state->x = x;
  state->y = y;
  state->risk = risk;

  if (heap->size == heap->count) {
    heap->size *= 2;
    heap->data = (struct state **)realloc(heap->data,
                                          sizeof(struct state *) * heap->size);
  }

  if (0 == heap->count) {
    heap->data[0] = state;
    heap->count++;
  } else {
    int ci = heap->count;
    heap->data[heap->count++] = state;
    siftUp(heap, ci);
  }
}

struct state *popHeap(minHeap *heap) {
  if (heap->count == 0) {
    return NULL;
  } else {
    swap(heap->data[0], heap->data[heap->count - 1]);
    heap->count--;
    siftDown(heap, 0);
    return heap->data[heap->count];
  }
}

void freeHeap(minHeap *heap) {
  int i = 0;
  for (i = 0; i < heap->count; i++) {
    free(heap->data[i]);
  }
  free(heap->data);
  free(heap);
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
      int8_t v = VAL(grid, y % grid->height, x % grid->width) +
                 (x / grid->width) + (y / grid->height);
      new->grid[y * new->width + x] = v > 9 ? v % 9 : v;
    }
  }

  return new;
}

int calculateRisk(grid_t *grid) {
  int x, y, risk = 0;

  minHeap *heap = initHeap(5);
  insertHeap(heap, 0, 0, 0);

  while (heap->count) {
    struct state *current = popHeap(heap);
    x = current->x;
    y = current->y;
    risk = current->risk;
    free(current);

    if (IS_VISITED(grid, x, y)) {
      continue;
    }
    SET_VISITED(grid, x, y);

    if (x == grid->width - 1 && y == grid->height - 1) {
      freeHeap(heap);
      return risk;
    }

    if (IS_INBOUND(grid, x - 1, y))
      insertHeap(heap, x - 1, y, risk + VAL(grid, x - 1, y));
    if (IS_INBOUND(grid, x + 1, y))
      insertHeap(heap, x + 1, y, risk + VAL(grid, x + 1, y));
    if (IS_INBOUND(grid, x, y - 1))
      insertHeap(heap, x, y - 1, risk + VAL(grid, x, y - 1));
    if (IS_INBOUND(grid, x, y + 1))
      insertHeap(heap, x, y + 1, risk + VAL(grid, x, y + 1));
  }

  freeHeap(heap);
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
