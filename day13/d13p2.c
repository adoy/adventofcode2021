#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#define DOT_BUFFER 1000

struct dotList {
  int count;
  struct {
    int x;
    int y;
  } dots[DOT_BUFFER];
};

void print(struct dotList *list) {
  int i, gridSize, width = 0, height = 0;
  int *grid;
  for (i = 0; i < list->count; i++) {
    if (list->dots[i].x > width)
      width = list->dots[i].x;
    if (list->dots[i].y > height)
      height = list->dots[i].y;
  }
  width++;
  height++;
  gridSize = width * height;
  grid = (int *)calloc(gridSize, sizeof(int));
  for (i = 0; i < list->count; i++) {
    grid[list->dots[i].y * width + list->dots[i].x] = 1;
  }
  for (i = 0; i < gridSize; i++) {
    if (0 == (i % width))
      printf("\n");
    printf("%c", grid[i] ? '#' : ' ');
  }
  printf("\n\n");
  free(grid);
}

int main() {
  struct dotList list = {0};
  char axe;
  int position, i;

#ifdef BENCH
  clock_t start = clock();
#endif

  while (fscanf(stdin, "%d,%d", &list.dots[list.count].x,
                &list.dots[list.count].y)) {
    list.count++;
  }

  while (fscanf(stdin, "fold along %c=%d%*c", &axe, &position) > 0) {
    if ('x' == axe) {
      for (i = 0; i < list.count; i++) {
        if (list.dots[i].x > position) {
          list.dots[i].x = position - (list.dots[i].x - position);
        }
      }
    } else {
      for (i = 0; i < list.count; i++) {
        if (list.dots[i].y > position) {
          list.dots[i].y = position - (list.dots[i].y - position);
        }
      }
    }
  }

  print(&list);

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  return 0;
}
