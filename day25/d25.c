#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define MAX_LINES 140
#define MAX_ROWS 140

typedef struct map {
  char map[MAX_LINES * MAX_ROWS + 2];
  int width;
  int height;
} map_t;

static void loadMap(map_t *map, FILE *fp) {
  char *str = map->map;
  map->width = 0;
  map->height = 0;

  while (map->height <= MAX_LINES && fgets(str, MAX_ROWS + 2, fp)) {
    if (!map->width) {
      map->width = strlen(map->map) - 1;
    }
    str += map->width;
    map->height++;
  }
}

static int step(map_t *map) {
  char new[MAX_LINES * MAX_ROWS + 2];
  int i, j, next, move = 0;
  memcpy(new, map->map, sizeof(new));

  for (j = 0; j < map->height; j++) {
    for (i = 0; i < map->width; i++) {
      next = (i + 1) % map->width;
      if (map->map[j * map->width + i] == '>' &&
          map->map[j * map->width + next] == '.') {
        new[j * map->width + next] = '>';
        new[j * map->width + i] = '.';
        i++;
        move++;
      }
    }
  }

  memcpy(map->map, new, sizeof(char) * map->width * map->height);
  for (i = 0; i < map->width; i++) {
    for (j = 0; j < map->height; j++) {
      next = (j + 1) % map->height;
      if (map->map[j * map->width + i] == 'v' &&
          map->map[next * map->width + i] == '.') {
        new[j * map->width + i] = '.';
        new[next * map->width + i] = 'v';
        j++;
        move++;
      }
    }
  }

  memcpy(map->map, new, sizeof(char) * map->width * map->height);
  return move;
}

int main() {
  int result = 0;
  map_t map;
  loadMap(&map, stdin);

  do {
    result++;
  } while (step(&map));

#ifdef BENCH
  clock_t start = clock();
#endif

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", result);
  return 0;
}
