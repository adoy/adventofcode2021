CC=gcc
CFLAGS=-Wall -Wextra -ansi -O3

SRC = $(wildcard */*.c)
BIN = $(SRC:.c=)

all: $(BIN)

bench: CFLAGS += -DBENCH
bench: $(BIN)
	@for i in $(BIN); do \
		echo $$i; \
		cat `dirname $$i`/input.txt | $$i; \
		echo ;\
	done

% : %.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean :
	@rm -f $(BIN)
