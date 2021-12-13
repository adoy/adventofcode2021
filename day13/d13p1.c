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

int countUnique(struct dotList *list) {
  int result = 0, i, j;
  for (i = 0; i < list->count; i++) {
    int isUnique = 1;
    for (j = i + 1; j < list->count; j++) {
      if (list->dots[i].x == list->dots[j].x &&
          list->dots[i].y == list->dots[j].y) {
        isUnique = 0;
        break;
      }
    }
    result += isUnique;
  }

  return result;
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

  if (fscanf(stdin, "fold along %c=%d%*c", &axe, &position) > 0) {
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

  i = countUnique(&list);

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", i);
  return 0;
}
