#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

typedef struct {
  int8_t pos;
  int score;
} player_t;

int main() {
  int dieSum, c, lastRoll = 0, result = 0, i = 1;
  player_t players[2] = {0};

#ifdef BENCH
  clock_t start = clock();
#endif

  fscanf(stdin, "Player 1 starting position: %hhd\n", &players[0].pos);
  fscanf(stdin, "Player 2 starting position: %hhd\n", &players[1].pos);

  do {
    i = !i;
    result++;
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

  result *= 3 * players[!i].score;

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Result=%d\n", result);

  return 0;
}
