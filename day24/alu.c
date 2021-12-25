#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#define IS_CONST 1
#define IS_VAR 2
#define IS_UNUSED 3

typedef struct {
  char *input;
  int var[4];
} state;

typedef int (*handler)(state *s, int *, int *);

typedef struct {
  int op_type;
  union {
    char var;
    int constant;
  };
} operand;

typedef struct alu_s {
  struct opcode {
    handler handler;
    char name[4];
    operand op1;
    operand op2;
  } opcode[255];
  int size;
} alu;

static int op_add(state *UNUSED(s), int *op1, int *op2) {
  *op1 += *op2;
  return 1;
}

static int op_div(state *UNUSED(s), int *op1, int *op2) {
  *op1 /= *op2;
  return 1;
}

static int op_eql(state *UNUSED(s), int *op1, int *op2) {
  *op1 = (*op1 == *op2);
  return 1;
}

static int op_mul(state *UNUSED(s), int *op1, int *op2) {
  if (op2 == 0) {
    return 0;
  }
  *op1 *= *op2;
  return 1;
}

static int op_mod(state *UNUSED(s), int *op1, int *op2) {
  *op1 %= *op2;
  if (op2 == 0) {
    return 0;
  }
  return 1;
}

static int op_inp(state *s, int *op1, int *UNUSED(op2)) {
  *op1 = *s->input - '0';
  s->input++;
  return 1;
}

static void compile(alu *alu, FILE *fp) {
  int numArgs;
  alu->size = 0;

  while (fscanf(fp, "%s ", alu->opcode[alu->size].name) > 0) {
    numArgs = 0;
    switch (alu->opcode[alu->size].name[0]) {
    case 'a':
      alu->opcode[alu->size].handler = op_add;
      numArgs = 2;
      break;
    case 'd':
      alu->opcode[alu->size].handler = op_div;
      numArgs = 2;
      break;
    case 'e':
      alu->opcode[alu->size].handler = op_eql;
      numArgs = 2;
      break;
    case 'i':
      alu->opcode[alu->size].handler = op_inp;
      numArgs = 1;
      break;
    case 'm':
      alu->opcode[alu->size].handler =
          (alu->opcode[alu->size].name[1] == 'u') ? op_mul : op_mod;
      numArgs = 2;
      break;
    }
    alu->opcode[alu->size].op1.op_type = IS_VAR;
    fscanf(fp, "%c ", &alu->opcode[alu->size].op1.var);
    if (numArgs > 1) {
      if (fscanf(fp, "%d", &alu->opcode[alu->size].op2.constant) > 0) {
        alu->opcode[alu->size].op2.op_type = IS_CONST;
      } else if (fscanf(fp, "%c", &alu->opcode[alu->size].op2.var)) {
        alu->opcode[alu->size].op2.op_type = IS_VAR;
      }
    } else {
      alu->opcode[alu->size].op2.op_type = IS_UNUSED;
    }
    alu->size++;
  }
}

static int *getOperandPtr(state *s, operand *op) {
  if (op->op_type == IS_VAR) {
    return &s->var[op->var - 'w'];
  } else if (op->op_type == IS_CONST) {
    return &op->constant;
  }
  return NULL;
}

static int execute(alu *alu, state *s) {
  int i;
  for (i = 0; i < alu->size; i++) {
    if (!alu->opcode[i].handler(s, getOperandPtr(s, &alu->opcode[i].op1), getOperandPtr(s, &alu->opcode[i].op2))) {
      return 0;
    }
  }
  return 1;
}

int main() {
  char *modelNumbers[2] = {"65984919997939", "11211619541713"};
  alu alu;
  size_t i;

#ifdef BENCH
  clock_t start = clock();
#endif

  compile(&alu, stdin);
  for (i = 0; i < sizeof(modelNumbers) / sizeof(char *); i++) {
    state state = {modelNumbers[i], {0}};
    execute(&alu, &state);
    printf("input: %s, w=%d, x=%d, y=%d, z: %d\n", modelNumbers[i], state.var[0], state.var[1], state.var[2], state.var[3]);
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
#endif

  return 0;
}
