#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#define LINE_BUFFER_SIZE 150
#define MAX_LINE_COUNT 110

#define push(stack, c) (stack.stack[stack.size++] = (c))
#define pop(stack) (stack.stack[--stack.size])

typedef struct stack {
  int size;
  char stack[LINE_BUFFER_SIZE];
} stack_t;

static long getLineScore(char *str) {
  long score = 0;
  stack_t stack = {0};

  while (*str) {
    switch (*str) {
      case '(':
      case '[':
      case '{':
      case '<':
        push(stack, *str);
        break;
      case ')':
        if (pop(stack) != '(')
          return 0;
        break;
      case ']':
        if (pop(stack) != '[')
          return 0;
        break;
      case '}':
        if (pop(stack) != '{')
          return 0;
        break;
      case '>':
        if (pop(stack) != '<')
          return 0;
        break;
    }
    str++;
  }

  while (stack.size) {
    score *= 5;
    switch (pop(stack)) {
      case '(':
        score += 1;
        break;
      case '[':
        score += 2;
        break;
      case '{':
        score += 3;
        break;
      case '<':
        score += 4;
        break;
    }
  }

  return score;
}

static int cmp(const void *a, const void *b) {
  if (*(long *)a - *(long *)b < 0)
    return -1;
  if (*(long *)a - *(long *)b > 0)
    return 1;
  return 0;
}

int main() {
  char line[LINE_BUFFER_SIZE + 2];
  long incompleteScores[MAX_LINE_COUNT];
  int incompleteCount = 0;
  long score;

#ifdef BENCH
  clock_t start = clock();
#endif

  while (fgets(line, LINE_BUFFER_SIZE + 2, stdin)) {
    if ((score = getLineScore(line))) {
      incompleteScores[incompleteCount++] = score;
    }
  }

  qsort(incompleteScores, incompleteCount, sizeof(long), cmp);

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %ld\n", incompleteScores[incompleteCount / 2]);

  return 0;
}
