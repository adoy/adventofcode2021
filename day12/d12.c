#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define isSmall(c) ((c)->name[0] >= 'a')

#define IS_START 1
#define IS_END 2

struct cave {
  char *name;
  char flags;
  struct caveList *connections;
};

struct caveList {
  struct cave *cave;
  struct caveList *next;
};

typedef struct cave cave_t;
typedef struct caveList caveList_t;

static void addCaveToList(caveList_t **list, cave_t *cave) {
  caveList_t *new = (caveList_t *)malloc(sizeof(caveList_t));
  new->cave = cave;
  new->next = *list;

  *list = new;
}

static cave_t *findCaveInList(caveList_t *list, const char *name) {
  while (list) {
    if (0 == strcmp(name, list->cave->name))
      return list->cave;
    list = list->next;
  };

  return NULL;
}

static cave_t *findOrCreateCaveInList(caveList_t **list, const char *name) {
  cave_t *cave = findCaveInList(*list, name);

  if (NULL == cave) {
    cave = (cave_t *)malloc(sizeof(cave_t));
    cave->name = strdup(name);
    cave->connections = NULL;
    if (0 == strcmp("start", name))
      cave->flags = IS_START;
    else if (0 == strcmp("end", name))
      cave->flags = IS_END;
    else
      cave->flags = 0;
    addCaveToList(list, cave);
  }

  return cave;
}

static int isCaveInList(caveList_t *list, const cave_t *cave) {
  while (list) {
    if (list->cave == cave)
      return 1;
    list = list->next;
  }

  return 0;
}

static caveList_t *initCaveList(FILE *fp) {
  caveList_t *list = NULL;
  char name1[10];
  char name2[10];

  while (fscanf(fp, "%[^-]s", name1) == 1 && fscanf(fp, "-%s\n", name2) == 1) {
    cave_t *cave1 = findOrCreateCaveInList(&list, name1);
    cave_t *cave2 = findOrCreateCaveInList(&list, name2);
    if (IS_START != cave2->flags)
      addCaveToList(&cave1->connections, cave2);
    if (IS_START != cave1->flags)
      addCaveToList(&cave2->connections, cave1);
  }

  return list;
}

static void freeCaveList(caveList_t *, int);

static void freeCave(cave_t *cave) {
  free(cave->name);
  freeCaveList(cave->connections, 0);
  free(cave);
}

static void freeCaveList(caveList_t *list, int fc) {
  caveList_t *e;
  while ((e = list)) {
    list = list->next;
    if (fc)
      freeCave(e->cave);
    free(e);
  }
}

static int visit(cave_t *cave, caveList_t *visited, int twice) {
  int r = 0, fv = 0;
  caveList_t *connections = cave->connections;

  if (0 == strcmp(cave->name, "end"))
    return 1;
  if (isSmall(cave)) {
    if (isCaveInList(visited, cave)) {
      if (twice)
        return 0;
      twice = 1;
    } else {
      addCaveToList(&visited, cave);
      fv = 1;
    }
  }

  while (connections) {
    r += visit(connections->cave, visited, twice);
    connections = connections->next;
  }

  if (fv) {
    free(visited);
  }

  return r;
}

int main() {
  int result1, result2;
#ifdef BENCH
  clock_t start = clock();
#endif
  caveList_t *caves = initCaveList(stdin);
  cave_t *cave = findCaveInList(caves, "start");
  result1 = visit(cave, NULL, 1);
  result2 = visit(cave, NULL, 0);
  freeCaveList(caves, 1);

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result1: %d\n", result1);
  printf("Result2: %d\n", result2);

  return 0;
}
