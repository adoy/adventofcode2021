#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#define LINE_BUFFER 500

struct line {
  int x1, y1, x2, y2;
};

int main() {
  struct line lines[LINE_BUFFER];
  int lineCount = 0;
  int width = 0, height = 0;
  int i, result = 0;
  int distance, xinc, yinc;
  int *grid;

#ifdef BENCH
  clock_t start = clock();
#endif

  while (fscanf(stdin, "%d,%d -> %d,%d", &lines[lineCount].x1, &lines[lineCount].y1, &lines[lineCount].x2, &lines[lineCount].y2) == 4) {
    if (lines[lineCount].x1 > width) width = lines[lineCount].x1;
    if (lines[lineCount].x2 > width) width = lines[lineCount].x2;
    if (lines[lineCount].y1 > height) height = lines[lineCount].y1;
    if (lines[lineCount].y2 > height) height = lines[lineCount].y2;
    lineCount++;
  }
  width++;
  height++;

  grid = (int *)calloc(width * height, sizeof(int));

  for (lineCount--; lineCount >= 0; lineCount--) {
    struct line *line = &lines[lineCount];
    distance = line->x2 - line->x1;
    xinc = distance ? distance / abs(distance) : 0;
    distance = line->y2 - line->y1;
    yinc = distance ? distance / abs(distance) : 0;
    grid[line->x1 + width * line->y1]++;
    while (line->x1 != line->x2 || line->y1 != line->y2) {
      line->x1 += xinc;
      line->y1 += yinc;
      grid[line->x1 + width * line->y1]++;
    }
  }

  for (i = width * height - 1; i >= 0; i--) {
    if (grid[i] > 1) result++;
  }

  free(grid);

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", result);

  return 0;
}
