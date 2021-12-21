#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int8_t pos;
  int score;
} player_t;

int main() {
  int dieSum, c, lastRoll = 0, round = 0, i = 1;
  player_t players[2] = {0};

  fscanf(stdin, "Player 1 starting position: %hhd\n", &players[0].pos);
  fscanf(stdin, "Player 2 starting position: %hhd\n", &players[1].pos);

  do {
    i = !i;
    round++;
    dieSum = 0;
    for (c = 0; c < 3; c++) {
      if (++lastRoll > 100) {
        lastRoll = 1;
      }
      dieSum += lastRoll;
    }
    players[i].pos = (dieSum + players[i].pos - 1) % 10 + 1;
    players[i].score += players[i].pos;
  } while (players[i].score < 1000);

  printf("Result=%d\n", players[!i].score * round * 3);

  return 0;
}
