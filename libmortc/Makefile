PREFIX = /usr/local

SRC = $(shell find src -name '*.c')
OBJ = $(patsubst src/%.c,obj/%.o,$(SRC))
HDR = $(shell find include -name '*.h')

CFLAGS ?= -g -Wall -Wextra -Wno-unused-parameter
VGFLAGS ?= --quiet --leak-check=full --show-leak-kinds=all --track-origins=yes

libmortc.so: $(OBJ) $(HDR)
	$(CC) -shared -fPIC -Iinclude -o $@ $(OBJ)

obj/%.o: src/%.c $(HDR)
	@mkdir -p $(dir $@)
	$(CC) -c -fPIC -Iinclude $(CFLAGS) -o $@ $<

install:
	cp libmortc.so $(PREFIX)/lib/$(TARGET)
	cp -r include/mortc $(PREFIX)/include

test: libmortc.so
	@$(MAKE) -C test >/dev/null
	cd test && valgrind $(VGFLAGS) ./test

clean:
	rm -f libmortc.so
	rm -rf obj
	$(MAKE) -C test clean 2>&1

.PHONY: install test clean
