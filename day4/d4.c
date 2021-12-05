#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER 500
#define DRAW_BUFFER 101
#define BOARD_COUNT 100
#define BOARD_LINES 5
#define BOARD_ROWS 5
#define BOARD_CELLS (BOARD_LINES * BOARD_ROWS)

#define getCell(board, line, row) board->cells[line * BOARD_LINES + row]

typedef struct Cell {
  int val;
  char checked;
} Cell;

typedef struct Board {
  struct Cell *cells;
  char won;
  int score;
} Board;

int validateBoardLine(Board *board, int line) {
  int row;
  for (row = 0; row < BOARD_ROWS; row++) {
    if (!getCell(board, line, row).checked) {
      return 0;
    }
  }
  return 1;
}

int validateBoardColumn(Board *board, int column) {
  int line;
  for (line = 0; line < BOARD_LINES; line++) {
    if (!getCell(board, line, column).checked) {
      return 0;
    }
  }
  return 1;
}

int computeScore(Board board, int number) {
  int result = 0, i;
  for (i = 0; i < BOARD_CELLS; i++) {
    if (!board.cells[i].checked)
      result += board.cells[i].val;
  }

  return result * number;
}

int validateBoard(Board *board, int number) {
  int i;
  for (i = 0; 0 == board->won && i < BOARD_LINES; i++) {
    if (validateBoardLine(board, i)) {
      board->won = 1;
      board->score = computeScore(*board, number);
    }
  }
  for (i = 0; 0 == board->won && i < BOARD_ROWS; i++) {
    if (validateBoardColumn(board, i)) {
      board->won = 1;
      board->score = computeScore(*board, number);
    }
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
  Board *winner = NULL, *loser;

  fscanf(stdin, "%d", draw++);
  while (fscanf(stdin, ",%d", draw) > 0) draw++;
  *draw = -1;

  while (fscanf(stdin, "%d", &cells[cellCount].val) > 0) {
    cells[cellCount++].checked = 0;
  }
  boardCount = cellCount / BOARD_CELLS;

  for (i = 0; i < boardCount; i++) {
    boards[i].won = 0;
    boards[i].cells = &cells[i * BOARD_CELLS];
  }

  draw = draws;
  while (*draw != -1) {
    for (i = 0; i < cellCount; i++) {
      if (*draw == cells[i].val) {
        cells[i].checked = 1;
      }
    }

    for (i = 0; i < boardCount; i++) {
      if (!boards[i].won && validateBoard(&boards[i], *draw)) {
        if (NULL == winner) winner = &boards[i];
        loser = &boards[i];
      }
    }
    draw++;
  }

  if (winner) {
    printf("Winner board score: %d\n", winner->score);
    printf("Loser board score: %d\n", loser->score);
  }

  return 0;
}
