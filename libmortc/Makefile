PREFIX = /usr/local

SRC = $(shell find src -name '*.c')
OBJ = $(patsubst src/%.c,obj/%.o,$(SRC))
HDR = $(shell find include -name '*.h')

CFLAGS ?= -g

libmortc.so: $(OBJ) $(HDR)
	$(CC) -shared -Iinclude -o $@ $(OBJ)

obj/%.o: src/%.c $(HDR)
	@mkdir -p $(dir $@)
	$(CC) -c -Iinclude $(CFLAGS) -o $@ $<

install:
	cp libmortc.so $(PREFIX)/lib/$(TARGET)
	cp -r include/mortc $(PREFIX)/include

test: libmortc.so
	$(MAKE) --no-print-directory -C test run

clean:
	rm -f libmortc.so
	rm -rf obj
	$(MAKE) -C test clean

.PHONY: install test clean
