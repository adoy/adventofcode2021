CC=gcc
CFLAGS=-Wall -Wextra -ansi -O3

SRC = $(wildcard */*.c)
BIN = $(SRC:.c=)

all: $(BIN)

bench: CFLAGS += -DBENCH
bench: $(BIN)

% : %.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean :
	@rm -f $(BIN)
