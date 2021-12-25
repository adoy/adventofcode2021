#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define HALLWAY_SIZE 11
#define MAX_DEPTH 4
#define LINE_BUFFER 15

#define parentIndex(i) (((i)-1) / 2)
#define leftChildIndex(i) (2 * (i) + 1)
#define rightChildIndex(i) (2 * (i) + 2)
#define swap(a, b)                                                             \
  do {                                                                         \
    struct step *tmp = a;                                                      \
    a = b;                                                                     \
    b = tmp;                                                                   \
  } while (0)

#define IS_AMPHIPOD(c) ((c) == 'A' || (c) == 'B' || (c) == 'C' || (c) == 'D')
#define IN_OWN_ROOM(c, r) ((c) - 'A' == r)
#define ROOM(c) ((c) - 'A')
#define COST(c) (energyCost[(c) - 'A'])
#define ALLOWED ((int) (sizeof(allowed) / sizeof(int)))

int rooms[] = {2, 4, 6, 8};
int energyCost[] = {1, 10, 100, 1000};
int allowed[] = {0, 1, 3, 5, 7, 9, 10};

typedef struct burrow_s {
  char hallway[HALLWAY_SIZE + 1];
  char siderooms[4 * MAX_DEPTH];
  int roomDepth;
} burrow;

typedef struct {
  int size;
  int count;
  struct step {
    burrow *burrow;
    int energy;
  } * *data;
} minHeap;

static minHeap *initHeap(size_t initialSize) {
  minHeap *heap = (minHeap *)malloc(sizeof(minHeap));
  heap->size = initialSize;
  heap->count = 0;
  heap->data = (struct step **)malloc(sizeof(struct step *) * heap->size);

  return heap;
}

static void siftUp(minHeap *heap, int ci) {
  int pi = parentIndex(ci);
  while (pi >= 0 && heap->data[pi]->energy > heap->data[ci]->energy) {
    swap(heap->data[pi], heap->data[ci]);
    ci = pi;
    pi = parentIndex(ci);
  }
}

static void siftDown(minHeap *heap, int pi) {
  int lci = leftChildIndex(pi);

  while (lci < heap->count) {
    int sci = lci;
    int rci = rightChildIndex(pi);
    if (rci < heap->count && heap->data[rci]->energy < heap->data[lci]->energy) {
      sci = rci;
    }
    if (heap->data[sci]->energy < heap->data[pi]->energy) {
      swap(heap->data[sci], heap->data[pi]);
      pi = sci;
      lci = leftChildIndex(pi);
    } else {
      break;
    }
  }
}

static void insertHeap(minHeap *heap, burrow *b, int energy) {
  struct step *step = (struct step *)malloc(sizeof(struct step));
  step->burrow = b;
  step->energy = energy;

  if (heap->size == heap->count) {
    heap->size *= 2;
    heap->data = (struct step **)realloc(heap->data, sizeof(struct step *) * heap->size);
  }

  if (0 == heap->count) {
    heap->data[0] = step;
    heap->count++;
  } else {
    int ci = heap->count;
    heap->data[heap->count++] = step;
    siftUp(heap, ci);
  }
}

static struct step *popHeap(minHeap *heap) {
  if (heap->count == 0) {
    return NULL;
  } else {
    swap(heap->data[0], heap->data[heap->count - 1]);
    heap->count--;
    siftDown(heap, 0);
    return heap->data[heap->count];
  }
}

static void freeHeap(minHeap *heap) {
  int i = 0;
  for (i = 0; i < heap->count; i++) {
    free(heap->data[i]->burrow);
    free(heap->data[i]);
  }
  free(heap->data);
  free(heap);
}

static burrow *burrowDup(burrow *b) {
  burrow *new = (burrow *)malloc(sizeof(burrow));
  memcpy(new, b, sizeof(burrow));
  return new;
}

static burrow *initBurrow(FILE *fp) {
  char line[LINE_BUFFER];
  int i;
  burrow *b = (burrow *)malloc(sizeof(burrow));
  fgets(line, LINE_BUFFER, fp);
  fgets(line, LINE_BUFFER, fp);
  memcpy(b->hallway, line + 1, sizeof(b->hallway));
  b->hallway[HALLWAY_SIZE] = '\0';
  b->roomDepth = 0;
  do {
    fgets(line, LINE_BUFFER, fp);
    if ('#' == line[rooms[0] + 1])
      break;
    for (i = 0; i < 4; i++) {
      b->siderooms[4 * b->roomDepth + i] = line[rooms[i] + 1];
    }
    b->roomDepth++;
  } while (b->roomDepth < MAX_DEPTH);
  return b;
}

static int isHallwayEmpty(burrow *b, int from, int to, int inHallway) {
  if (inHallway && from == to)
    return 0;
  if (to > from) {
    for (from += inHallway; from <= to; from++)
      if (b->hallway[from] != '.')
        return 0;
  } else {
    for (from -= inHallway; from >= to; from--)
      if (b->hallway[from] != '.')
        return 0;
  }
  return 1;
}

static int canMoveFromRoom(burrow *b, int depth, int room) {
  char amphipod = b->siderooms[4 * depth + room];
  if (!IN_OWN_ROOM(amphipod, room)) {
    while (depth != 0) {
      if (b->siderooms[--depth * 4 + room] != '.')
        return 0;
    }
    return 1;
  }
  while (depth < b->roomDepth) {
    if (b->siderooms[depth * 4 + room] != amphipod)
      return 1;
    depth++;
  }
  return 0;
}

static int moveFromRoomToRoom(burrow *b, int depth, int room) {
  char amphipod = b->siderooms[depth * 4 + room];
  int c = depth + 1 + abs(room - ROOM(amphipod)) * 2;
  b->siderooms[depth * 4 + room] = '.';
  room = ROOM(amphipod);
  for (depth = b->roomDepth - 1; depth >= 0; depth--) {
    if (b->siderooms[depth * 4 + room] == '.') {
      b->siderooms[depth * 4 + room] = amphipod;
      break;
    }
  }
  return COST(amphipod) * (c + depth + 1);
}

static int canMoveToRoom(burrow *b, char amphipod) {
  int depth, room = ROOM(amphipod);
  for (depth = b->roomDepth - 1; depth >= 0; depth--) {
    if (b->siderooms[depth * 4 + room] == '.')
      return 1;
    else if (b->siderooms[depth * 4 + room] != amphipod)
      return 0;
  }

  return 1;
}

static int moveToRoom(burrow *b, int i) {
  char amphipod = b->hallway[i];
  int depth, room = ROOM(amphipod);
  for (depth = b->roomDepth - 1; depth >= 0; depth--) {
    if (b->siderooms[depth * 4 + room] == '.') {
      b->siderooms[depth * 4 + room] = amphipod;
      break;
    }
  }
  b->hallway[i] = '.';

  return COST(amphipod) * (abs(i - rooms[room]) + depth + 1);
}

static int moveFromRoomToHallway(burrow *b, int depth, int room, int pos) {
  char amphipod = b->siderooms[depth * 4 + room];
  b->siderooms[depth * 4 + room] = '.';
  b->hallway[pos] = amphipod;

  return COST(amphipod) * (depth + 1 + abs(pos - rooms[room]));
}

static int isSolved(burrow *b) {
  int r, d;
  for (r = 0; r < 4; r++)
    for (d = 0; d < b->roomDepth; d++)
      if (b->siderooms[4 * d + r] != r + 'A')
        return 0;

  return 1;
}

int solve(burrow *b) {
  int d, i, r;
  int energy = 0;
  burrow *bcopy;

  minHeap *heap = initHeap(100);
  insertHeap(heap, b, energy);

  while (heap->count) {
    struct step *step;
    step = popHeap(heap);
    b = step->burrow;
    energy = step->energy;
    free(step);

    if (isSolved(b)) {
      freeHeap(heap);
      free(b);
      return energy;
    }

    for (i = 0; i < HALLWAY_SIZE; i++) {
#define current (b->hallway[i])
      if (IS_AMPHIPOD(current) && canMoveToRoom(b, current) && isHallwayEmpty(b, i, rooms[ROOM(current)], 1)) {
        bcopy = burrowDup(b);
        insertHeap(heap, bcopy, energy + moveToRoom(bcopy, i));
      }
#undef current
    }

    for (d = 0; d < b->roomDepth; d++) {
      for (r = 0; r < 4; r++) {
#define current (b->siderooms[4 * d + r])
        if (IS_AMPHIPOD(current) && canMoveFromRoom(b, d, r)) {
          if (canMoveToRoom(b, current) && isHallwayEmpty(b, rooms[r], rooms[ROOM(current)], 0)) {
            bcopy = burrowDup(b);
            insertHeap(heap, bcopy, energy + moveFromRoomToRoom(bcopy, d, r));
          } else {
            for (i = 0; i < ALLOWED; i++) {
              if (isHallwayEmpty(b, rooms[r], allowed[i], 0)) {
                bcopy = burrowDup(b);
                insertHeap(heap, bcopy, energy + moveFromRoomToHallway(bcopy, d, r, allowed[i]));
              }
            }
          }
        }
#undef current
      }
    }
    free(b);
  }

  return -1;
}

int main() {
#ifdef BENCH
  clock_t start = clock();
#endif
  int result = solve(initBurrow(stdin));

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", result);
  return 0;
}
