#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#define LINE_BUFFER 300
#define MAX_NUMBERS 100

#define TYPE_LEAF 1 << 1
#define TYPE_NODE 1 << 2
#define LEFT_NODE 1 << 3
#define RIGHT_NODE 1 << 4

#define IS_LEAF(n) ((n)->flags & TYPE_LEAF)
#define IS_NODE(n) ((n)->flags & TYPE_NODE)
#define IS_LEFT_NODE(n) ((n)->flags & LEFT_NODE)
#define IS_RIGHT_NODE(n) ((n)->flags & RIGHT_NODE)

#define setLeft(p, c)                                                          \
  do {                                                                         \
    (p)->left = (c);                                                           \
    (p)->left->parent = p;                                                     \
    (p)->left->flags |= LEFT_NODE;                                             \
  } while (0)

#define setRight(p, c)                                                         \
  do {                                                                         \
    (p)->right = (c);                                                          \
    (p)->right->parent = p;                                                    \
    (p)->right->flags |= RIGHT_NODE;                                           \
  } while (0)

typedef struct snum {
  int flags;
  union {
    int val;
    struct {
      struct snum *left;
      struct snum *right;
    };
  };
  struct snum *parent;
} snailfishNumber;

static snailfishNumber *sndup(snailfishNumber *src) {
  snailfishNumber *sn = (snailfishNumber *)malloc(sizeof(snailfishNumber));
  sn->flags = src->flags;
  if (IS_LEAF(sn)) {
    sn->val = src->val;
  } else {
    setLeft(sn, sndup(src->left));
    setRight(sn, sndup(src->right));
  }

  return sn;
}

static void freeSnailfishNumber(snailfishNumber *sn) {
  if (sn->flags & TYPE_NODE) {
    freeSnailfishNumber(sn->left);
    freeSnailfishNumber(sn->right);
  }
  free(sn);
}

static snailfishNumber *createNumber(int val) {
  snailfishNumber *sn = (snailfishNumber *)malloc(sizeof(snailfishNumber));
  sn->flags = TYPE_LEAF;
  sn->val = val;
  return sn;
}

static snailfishNumber *parseSnailfishNumber(char **str) {
  snailfishNumber *sn = (snailfishNumber *)malloc(sizeof(snailfishNumber));
  sn->flags = TYPE_NODE;
  sn->parent = NULL;
  (*str)++;
  if (**str == '[') {
    setLeft(sn, parseSnailfishNumber(str));
  } else {
    setLeft(sn, createNumber((**str)-'0'));
    (*str)++;
  }
  sn->left->parent = sn;
  sn->left->flags |= LEFT_NODE;
  (*str)++;
  if (**str == '[') {
    setRight(sn, parseSnailfishNumber(str));
  } else {
    setRight(sn, createNumber((**str)-'0'));
    (*str)++;
  }
  sn->right->parent = sn;
  sn->right->flags |= RIGHT_NODE;
  (*str)++;

  return sn;
}

static snailfishNumber *getNextLeaf(snailfishNumber *sn) {
  do {
    if (IS_LEFT_NODE(sn)) {
      sn = sn->parent->right;
      while (!IS_LEAF(sn)) {
        sn = sn->left;
      }
      return sn;
    }
  } while ((sn = sn->parent));

  return NULL;
}

static snailfishNumber *getPreviousLeaf(snailfishNumber *sn) {
  do {
    if (IS_RIGHT_NODE(sn)) {
      sn = sn->parent->left;
      while (!IS_LEAF(sn)) {
        sn = sn->right;
      }
      return sn;
    }
  } while ((sn = sn->parent));

  return NULL;
}

static int explode(snailfishNumber *sn, int depth) {
  if (IS_LEAF(sn->left) && IS_LEAF(sn->right) && depth >= 4) {
    snailfishNumber *z = createNumber(0);
    snailfishNumber *l;
    int n1 = sn->left->val;
    int n2 = sn->right->val;
    if (IS_LEFT_NODE(sn))
      setLeft(sn->parent, z);
    else
      setRight(sn->parent, z);

    if ((l = getPreviousLeaf(z)))
      l->val += n1;
    if ((l = getNextLeaf(z)))
      l->val += n2;

    freeSnailfishNumber(sn);
    return 1;
  }

  return (IS_NODE(sn->left) && explode(sn->left, depth + 1)) ||
         (IS_NODE(sn->right) && explode(sn->right, depth + 1));
}

static snailfishNumber *add(snailfishNumber *sn1, snailfishNumber *sn2) {
  snailfishNumber *sn = (snailfishNumber *)malloc(sizeof(snailfishNumber));
  sn->flags = TYPE_NODE;
  sn->parent = NULL;
  setLeft(sn, sn1);
  setRight(sn, sn2);
  return sn;
}

static int split(snailfishNumber *sn) {
  int n1, n2;
  if (IS_LEAF(sn->left)) {
    if (sn->left->val >= 10) {
      n1 = sn->left->val / 2;
      n2 = (sn->left->val & 1) ? n1 + 1 : n1;
      freeSnailfishNumber(sn->left);
      setLeft(sn, add(createNumber(n1), createNumber(n2)));
      return 1;
    }
  } else if (split(sn->left)) {
    return 1;
  }
  if (IS_LEAF(sn->right)) {
    if (sn->right->val >= 10) {
      n1 = sn->right->val / 2;
      n2 = (sn->right->val & 1) ? n1 + 1 : n1;
      freeSnailfishNumber(sn->right);
      setRight(sn, add(createNumber(n1), createNumber(n2)));
      return 1;
    }
  } else if (split(sn->right)) {
    return 1;
  }
  return 0;
}

static snailfishNumber *reduce(snailfishNumber *sn) {
  while (explode(sn, 0) || split(sn))
    ;
  return sn;
}

static int magnitude(snailfishNumber *sn) {
  if (IS_LEAF(sn)) {
    return sn->val;
  }

  return magnitude(sn->left) * 3 + magnitude(sn->right) * 2;
}

int main() {
  char line[LINE_BUFFER];
  snailfishNumber *sn[MAX_NUMBERS];
  snailfishNumber *tmp;
  int i, j, m, result1 = 0, count = 0, result2 = 0;
#ifdef BENCH
  clock_t start = clock();
#endif

  while (fgets(line, LINE_BUFFER, stdin)) {
    char *str = line;
    sn[count++] = reduce(parseSnailfishNumber(&str));
  }

  for (i = 0; i < count; i++) {
    for (j = i + 1; j < count; j++) {
      tmp = reduce(add(sndup(sn[i]), sndup(sn[j])));
      m = magnitude(tmp);
      freeSnailfishNumber(tmp);
      if (m > result2)
        result2 = m;

      tmp = reduce(add(sndup(sn[j]), sndup(sn[i])));
      m = magnitude(tmp);
      freeSnailfishNumber(tmp);
      if (m > result2)
        result2 = m;
    }
  }

  for (i = 1; i < count; i++) {
    sn[0] = reduce(add(sn[0], sn[i]));
  }
  result1 = magnitude(sn[0]);
  freeSnailfishNumber(sn[0]);

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result1: %d\nResult2: %d\n", result1, result2);

  return 0;
}
