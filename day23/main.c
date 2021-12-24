#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HALLWAY_SIZE 11
#define ROOMS_DEPTH 2
#define MAX_BURROW_WIDTH 

#define LINE_BUFFER 15

#define IS_AMPHIPOD(c) ((c)=='A' || (c) =='B' || (c) == 'C' || (c) == 'D')
#define GET_AMPHIPOD_COST(c) (cost[(c)-'A'])
#define IN_OWN_ROOM(c,r) ((c)-'A' == r)
#define ROOM(c) ((c)-'A')
#define ALLOWED (sizeof(allowed)/sizeof(int))

int rooms[] = { 2, 4, 6, 8 };
int cost[] = {1, 10, 100, 1000};
int allowed[] = {0, 1, 3, 5, 7, 9, 10};

typedef struct {
	char hallway[HALLWAY_SIZE+1];
	char siderooms[4 * ROOMS_DEPTH];
	int score;
} burrow;

void printBurrow(burrow *b) {
	int d, i;
	printf("#############\n");
	printf("#%11s#\n", b->hallway);
	for (d = 0; d < ROOMS_DEPTH; d++) {
		printf("%s", d ? "  ": "##");
		for (i = 0; i < 4; i++) {
			printf("#%c", b->siderooms[i*ROOMS_DEPTH+d]);
		}
		printf("%s", d ? "#\n" : "###\n");
	}
	printf("  #########\n");
}

void initBurrow(burrow *b) {
	char line[LINE_BUFFER];
	int d, i;
	fgets(line, LINE_BUFFER, stdin);
	fgets(line, LINE_BUFFER, stdin);
	memcpy(b->hallway, line+1, sizeof(b->hallway));
	b->hallway[HALLWAY_SIZE] = '\0';
	b->score = 0;
	for (d = 0; d < ROOMS_DEPTH; d++) {
		fgets(line, LINE_BUFFER, stdin);
		for (i = 0; i < 4; i++) {
			b->siderooms[i*ROOMS_DEPTH+d] = line[rooms[i]+1];
		}
	}
}

int isHallwayEmpty(burrow *b, int from, int to, int inHallway) {
	if (inHallway && from == to) return 0;
	if (to > from) { 
		for ( from+=inHallway ; from <= to ; from++)
			if (b->hallway[from] != '.') return 0;
	} else {
		for ( from-=inHallway ; from >= to ; from--)
			if (b->hallway[from] != '.') return 0;
	}
	return 1;
}

int canMoveFromRoom(burrow *b, int depth, int room) {
	char amphipod = b->siderooms[room * ROOMS_DEPTH + depth];
	if (!IN_OWN_ROOM(amphipod, room)) {
		while (depth != 0) {
			if (b->siderooms[room * ROOMS_DEPTH + --depth] != '.') 
				return 0;
		}
		return 1;
	}
	while (depth < ROOMS_DEPTH) {
		if (b->siderooms[room * ROOMS_DEPTH + depth] != amphipod) 
			return 1;
		depth++;
	}
	return 0;
}

int canGoToOwnRoom(burrow *b, char amphipod) {
	int depth, room = ROOM(amphipod);
	for (depth = ROOMS_DEPTH -1 ; depth >= 0; depth--) {
		if (b->siderooms[room * ROOMS_DEPTH + depth] == '.') return 1;
		else if (b->siderooms[room * ROOMS_DEPTH + depth] != amphipod) return 0;
	}

	return 1;
}

void solve(burrow b) {
	int d, i, j, r;
	for (i = 0; i < HALLWAY_SIZE; i++) {
#define current (b.hallway[i])
		if (IS_AMPHIPOD(current)) {
			if (canGoToOwnRoom(&b, current) && isHallwayEmpty(&b, i, rooms[ROOM(current)], 1)) {
				printf("Je suis %c dans le couloir going home\n", current);
			} else {
				for (j = 0; j < ALLOWED; j++) {
					if (isHallwayEmpty(&b, i, allowed[j], 1)) {
						printf("Je suis %c dans le couloir et je peu aller à %d\n", current, allowed[j]);
					}
				}
			}
		}
#undef current
	}
	for (d = 0; d < ROOMS_DEPTH; d++) {
		for (r = 0; r < 4; r++) {
#define current (b.siderooms[r*ROOMS_DEPTH+d])
			if (IS_AMPHIPOD(current) && canMoveFromRoom(&b, d, r)) {
				if (canGoToOwnRoom(&b, current) && isHallwayEmpty(&b, rooms[r], rooms[ROOM(current)], 0)) {
					printf("%c JE VAIS DANS MA ROOM\n", current);
				} else {
					for (i = 0; i < ALLOWED; i++) {
						if (isHallwayEmpty(&b, rooms[r], allowed[i], 0)) {
							printf("%c peu aller dans le couloir %d\n", current, allowed[i]);
						}
					}
				}
			}
#undef current
		}
	}
}

int main() {
	burrow b;
	initBurrow(&b);
	printBurrow(&b);
	solve(b);
	return 0;
}


