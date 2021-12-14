#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define STRING_BUFFER 25
#define toChar(x) ((x) + 'A')
#define toInt(x) ((x) - 'A')

typedef struct rule {
  int8_t e1, e2, e;
  struct rule *next;
} rule_t;

typedef struct pairCount {
  int8_t e1, e2;
  unsigned long count;
  struct pairCount *next;
} pairCount_t;

void freeRules(rule_t *rules) {
  rule_t *next;
  while (rules) {
    next = rules->next;
    free(rules);
    rules = next;
  }
}

int8_t findNewElement(rule_t *rules, int8_t e1, int8_t e2) {
  while (rules) {
    if (rules->e1 == e1 && rules->e2 == e2) {
      return rules->e;
    }
    rules = rules->next;
  }

  fprintf(stderr, "Unknown pair association '%c%c'\n", toChar(e1), toChar(e2));
  exit(1);
}

void freeCount(pairCount_t *count) {
  pairCount_t *next;
  while (count) {
    next = count->next;
    free(count);
    count = next;
  }
}

void incPairCount(pairCount_t **count, int8_t e1, int8_t e2, unsigned long inc) {
  pairCount_t *current = *count;
  while (current) {
    if (current->e1 == e1 && current->e2 == e2) {
      current->count += inc;
      return;
    }
    current = current->next;
  }

  current = (pairCount_t *)malloc(sizeof(pairCount_t));
  current->e1 = e1;
  current->e2 = e2;
  current->count = inc;
  current->next = *count;

  *count = current;
}

unsigned long computeResult(unsigned long *elements) {
  int8_t i;
  unsigned long max = 0;
  unsigned long min = ULONG_MAX;
  for (i = 0; i < 26; i++) {
    if (elements[i]) {
      if (elements[i] < min)
        min = elements[i];
      if (elements[i] > max)
        max = elements[i];
    }
  }
  return max - min;
}

int main(int argc, char **argv) {
  char str[STRING_BUFFER];
  char e1, e2, e;
  int8_t i, length, steps;
  unsigned long elements[26] = {0L};
  unsigned long result;
  rule_t *rules = NULL;
  pairCount_t *pairCount = NULL;

#ifdef BENCH
  clock_t start = clock();
  steps = (argc < 2) ? 40 : atoi(argv[1]);
#else
  if (argc < 2) {
    fprintf(stderr, "Usage: %s NUMBEROFSTEPS < input.txt\n", argv[0]);
    return 1;
  }
  steps = atoi(argv[1]);
#endif

  fscanf(stdin, "%s%*c%*c", str);
  length = strlen(str);
  for (i = 0; i < length; i++) {
    elements[toInt(str[i])]++;
  }

  while (fscanf(stdin, "%c%c -> %c\n", &e1, &e2, &e) > 0) {
    rule_t *rule = (rule_t *)malloc(sizeof(rule_t));
    rule->e1 = toInt(e1);
    rule->e2 = toInt(e2);
    rule->e = toInt(e);
    rule->next = rules;
    rules = rule;
  }

  for (i = 1; i < length; i++) {
    incPairCount(&pairCount, toInt(str[i - 1]), toInt(str[i]), 1);
  }

  for (i = 0; i < steps; i++) {
    pairCount_t *newPairCount = NULL;
    pairCount_t *current = pairCount;

    while (current) {
      int8_t r = findNewElement(rules, current->e1, current->e2);
      elements[r] += current->count;
      incPairCount(&newPairCount, current->e1, r, current->count);
      incPairCount(&newPairCount, r, current->e2, current->count);
      current = current->next;
    }

    freeCount(pairCount);
    pairCount = newPairCount;
  }

  result = computeResult(elements);
  freeRules(rules);
  freeCount(pairCount);

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %lu\n", result);

  return 0;
}
