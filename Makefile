ifeq ($(shell uname), Darwin)
CC=gcc-11
else
CC=cc
endif

CFLAGS=-Wall -Werror
DEBUG?=0

ifeq ($(DEBUG), 1)
CFLAGS+=-DDEBUG -g -O0 -fsanitize=address -fno-omit-frame-pointer
else
CFLAGS+=-O2
endif

INCLUDES=-I./include
AR=ar
ARFLAGS=rcs

OBJS=$(patsubst %.c, %.o, $(wildcard src/*.c))
PRGS=bfs transpose write_mtx
LIB=libuy.a

all: $(PRGS)

$(PRGS): %: prgs/%.o $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

lib: $(LIB)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

prgs/%.o: prgs/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

.PHONY: all clean purge

clean:
	rm -rf $(OBJS) prgs/*.o *.dSYM

purge: clean
	rm -rf $(LIB) $(PRGS)
