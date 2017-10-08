SRC = $(shell find . -name '*.c')
OBJ = $(patsubst %.c,obj/%.o,$(SRC))
HDR = $(shell find . -name '*.h')

CFLAGS ?= -g -Wall -Wextra -Wpedantic -Werror
FLAGS = $(CFLAGS) -I../include -Isrc
LINK = -L.. -Wl,-rpath -Wl,.. -lmortc

test: $(OBJ) $(HDR) ../libmortc.so
	$(CC) $(FLAGS) -o $@ $(OBJ) $(LINK)

obj/%.o: %.c $(HDR)
	@mkdir -p $(dir $@)
	$(CC) -c $(FLAGS) -o $@ $<

clean:
	rm -f test
	rm -rf obj

.PHONY: run clean
