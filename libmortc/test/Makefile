SRC = $(shell find . -name '*.c')
OBJ = $(patsubst %.c,obj/%.o,$(SRC))
HDR = $(shell find . -name '*.h')

CFLAGS ?= -g -Wall -Wextra -Wpedantic -Werror
FLAGS = $(CFLAGS) -I../include -Isrc
LINK = -L.. -lmortc -Wl,-rpath -Wl,..

VFLAGS ?= --leak-check=full --show-leak-kinds=all

test: $(OBJ) $(HDR) ../libmortc.so
	$(CC) $(FLAGS) $(LINK) -o $@ $(OBJ)

obj/%.o: %.c $(HDR)
	@mkdir -p $(dir $@)
	$(CC) -c $(FLAGS) -o $@ $<

run: test
	valgrind --quiet $(VFLAGS) ./test

clean:
	rm -f test
	rm -rf obj

.PHONY: run clean
