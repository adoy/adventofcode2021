#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define LINE_BUFFER 500
#define DRAW_BUFFER 101
#define BOARD_COUNT 100
#define BOARD_LINES 5
#define BOARD_ROWS 5
#define BOARD_CELLS (BOARD_LINES * BOARD_ROWS)

#define getCell(board, line, row) (board->cells[(line) * BOARD_LINES + (row)])

#define validateBoardLine(board, line)                                         \
  (getCell(board, line, 0).checked && getCell(board, line, 1).checked &&       \
   getCell(board, line, 2).checked && getCell(board, line, 3).checked &&       \
   getCell(board, line, 4).checked)
#define validateBoardColumn(board, column)                                     \
  (getCell(board, 0, column).checked && getCell(board, 1, column).checked &&   \
   getCell(board, 2, column).checked && getCell(board, 3, column).checked &&   \
   getCell(board, 4, column).checked)

#define BOARD_NUMBER(i) ((i) / BOARD_CELLS)
#define CELL_NUMBER(i) ((i) % 25)

typedef struct Cell {
  int val;
  char checked;
} Cell;

typedef struct Board {
  struct Cell *cells;
  char won;
  int score;
} Board;

int computeScore(Board board, int number) {
  int result = 0, i;
  for (i = 0; i < BOARD_CELLS; i++) {
    if (!board.cells[i].checked)
      result += board.cells[i].val;
  }

  return result * number;
}

int validateBoard(Board *board, int cell) {
  if (validateBoardLine(board, cell / 5) || validateBoardColumn(board, cell % 5)) {
    board->won = 1;
    board->score = computeScore(*board, board->cells[cell].val);
  }

  return board->won;
}

int main() {
  int draws[DRAW_BUFFER];
  int *draw = draws;

  Cell cells[BOARD_COUNT * BOARD_CELLS];
  int cellCount = 0;
  int boardCount = 0;
  int i;

  Board boards[BOARD_COUNT];
  Board *winner = NULL, *loser = NULL;

#ifdef BENCH
  clock_t start = clock();
#endif

  fscanf(stdin, "%d", draw++);
  while (fscanf(stdin, ",%d", draw) > 0) draw++;
  *draw = -1;

  while (fscanf(stdin, "%d", &cells[cellCount].val) > 0) {
    cells[cellCount++].checked = 0;
  }
  boardCount = BOARD_NUMBER(cellCount);

  for (i = 0; i < boardCount; i++) {
    boards[i].won = 0;
    boards[i].cells = &cells[i * BOARD_CELLS];
  }

  draw = draws;
  while (*draw != -1) {
    for (i = 0; i < cellCount; i++) {
      if (*draw == cells[i].val) {
        if (!boards[BOARD_NUMBER(i)].won) {
          cells[i].checked = 1;
          if (validateBoard(&boards[BOARD_NUMBER(i)], CELL_NUMBER(i))) {
            if (NULL == winner)
              winner = &boards[BOARD_NUMBER(i)];
            loser = &boards[BOARD_NUMBER(i)];
          }
        }
        i += BOARD_CELLS - CELL_NUMBER(i);
      }
    }
    draw++;
  }

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  if (winner) {
    printf("Winner board score: %d\n", winner->score);
    printf("Loser board score: %d\n", loser->score);
  }

  return 0;
}
