#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef BENCH
#include <time.h>
#endif

#define DIE_SIDES 3

int moves[] = {1, 3, 6, 7, 6, 3, 1};

typedef struct {
  int8_t pos;
  int8_t score;
} player_t;

typedef struct {
  unsigned long p1;
  unsigned long p2;
} results_t;

results_t play(player_t p1, player_t *p2) {
  int8_t i = 0, pos, score;
  results_t r = {0L, 0L}, r2;

  for (i = 0; i < (int8_t)(sizeof(moves) / sizeof(int)); i++) {
    pos = (p1.pos - 1 + i + DIE_SIDES) % 10 + 1;
    score = p1.score + pos;
    if (score >= 21) {
      r.p1 += moves[i];
      continue;
    }
    r2 = play(*p2, &(player_t){pos, p1.score + pos});
    r.p1 += r2.p2 * moves[i];
    r.p2 += r2.p1 * moves[i];
  }

  return r;
}

int main() {
  player_t players[2] = {0};
  results_t results;

#ifdef BENCH
  clock_t start = clock();
#endif

  fscanf(stdin, "Player 1 starting position: %hhd\n", &players[0].pos);
  fscanf(stdin, "Player 2 starting position: %hhd\n", &players[1].pos);

  results = play(players[0], &players[1]);

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Results=%ld\n", results.p1 > results.p2 ? results.p1 : results.p2);

  return 0;
}
