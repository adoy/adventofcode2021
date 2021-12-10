#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#define LINE_BUFFER_SIZE 255

#define SEG_A (1 << 0)
#define SEG_B (1 << 1)
#define SEG_C (1 << 2)
#define SEG_D (1 << 3)
#define SEG_E (1 << 4)
#define SEG_F (1 << 5)
#define SEG_G (1 << 6)
#define LOOKUP_SIZE ((1 << 7) - 1)

typedef struct line {
  int signals[10];
  int digits[4];
  int mapping[10];
  int lookup[LOOKUP_SIZE];
} line_t;

int countSetBits(int n) {
  int count = 0;
  while (n) {
    n = n & (n - 1);
    count++;
  }

  return count;
}

void parseLine(char *str, line_t *line) {
  int i;
  char inDigits = 0;
  int offset = 0;

  for (i = 0; i < LINE_BUFFER_SIZE && '\n' != str[i] && 0 != str[i]; i++) {
    switch (str[i]) {
    case 'a':
      *((inDigits) ? &line->digits[offset] : &line->signals[offset]) |= SEG_A;
      break;
    case 'b':
      *((inDigits) ? &line->digits[offset] : &line->signals[offset]) |= SEG_B;
      break;
    case 'c':
      *((inDigits) ? &line->digits[offset] : &line->signals[offset]) |= SEG_C;
      break;
    case 'd':
      *((inDigits) ? &line->digits[offset] : &line->signals[offset]) |= SEG_D;
      break;
    case 'e':
      *((inDigits) ? &line->digits[offset] : &line->signals[offset]) |= SEG_E;
      break;
    case 'f':
      *((inDigits) ? &line->digits[offset] : &line->signals[offset]) |= SEG_F;
      break;
    case 'g':
      *((inDigits) ? &line->digits[offset] : &line->signals[offset]) |= SEG_G;
      break;
    case ' ':
      offset++;
      break;
    case '|':
      offset = 0;
      inDigits = 1;
      i++;
      break;
    }
  }
}

int processLine(line_t *l) {
  int i, d = 0;

#define SKIP_ALREADY_MAPPED()                                                  \
  if (-1 == l->signals[i])                                                     \
  continue

#define CONFIRM_MAPPING(result)                                                \
  do {                                                                         \
    l->mapping[result] = l->signals[i];                                        \
    l->lookup[l->signals[i]] = result;                                         \
    l->signals[i] = -1;                                                        \
  } while (0)

  for (i = 0; i < 10; i++) {
    switch (countSetBits(l->signals[i])) {
    case 2:
      CONFIRM_MAPPING(1);
      break;
    case 4:
      CONFIRM_MAPPING(4);
      break;
    case 3:
      CONFIRM_MAPPING(7);
      break;
    case 7:
      CONFIRM_MAPPING(8);
      break;
    }
  }

  for (i = 0; i < 10; i++) {
    SKIP_ALREADY_MAPPED();
    if (countSetBits(l->signals[i]) == 6 &&
        (l->signals[i] & l->mapping[7]) == l->mapping[7] &&
        (l->signals[i] & l->mapping[4]) == l->mapping[4]) {
      CONFIRM_MAPPING(9);
      break;
    }
  }

  for (i = 0; i < 10; i++) {
    SKIP_ALREADY_MAPPED();
    if (countSetBits(l->signals[i]) == 5 &&
        countSetBits(l->signals[i] & l->mapping[9]) == 4) {
      CONFIRM_MAPPING(2);
      break;
    }
  }

  for (i = 0; i < 10; i++) {
    SKIP_ALREADY_MAPPED();
    if (countSetBits(l->signals[i]) == 6 &&
        countSetBits(l->signals[i] & l->mapping[7]) == 3) {
      CONFIRM_MAPPING(0);
      break;
    }
  }

  for (i = 0; i < 10; i++) {
    SKIP_ALREADY_MAPPED();
    if (countSetBits(l->signals[i]) == 6) {
      CONFIRM_MAPPING(6);
      break;
    }
  }

  for (i = 0; i < 10; i++) {
    SKIP_ALREADY_MAPPED();
    if (countSetBits(l->signals[i]) == 5 &&
        (l->signals[i] & l->mapping[6]) == l->signals[i]) {
      CONFIRM_MAPPING(5);
      break;
    }
  }

  for (i = 0; i < 10; i++) {
    SKIP_ALREADY_MAPPED();
    CONFIRM_MAPPING(3);
  }

#undef SKIP_ALREADY_MAPPED
#undef CONFIRM_MAPPING

  d = 0;
  for (i = 0; i < 4; i++) {
    d = d * 10 + l->lookup[l->digits[i]];
  }

  return d;
}

int main() {
  char str[LINE_BUFFER_SIZE];
  int result = 0;

#ifdef BENCH
  clock_t start = clock();
#endif

  while (fgets(str, LINE_BUFFER_SIZE, stdin)) {
    line_t l = {0};
    parseLine(str, &l);
    result += processLine(&l);
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result: %d\n", result);

  return 0;
}
