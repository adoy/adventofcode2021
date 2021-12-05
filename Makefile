CC=gcc
CFLAGS=-Wall -Wextra -O3

SRC = $(wildcard */*.c)
BIN = $(SRC:.c=)

all: $(BIN)

% : %.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean :
	@rm -f $(BIN)
