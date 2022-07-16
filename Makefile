CC=gcc-11
CFLAGS=-Wall -O2
# CFLAGS=-Wall -g -O0 -fsanitize=address -fno-omit-frame-pointer
INCLUDES=-I./include
AR=ar
ARFLAGS=rcs
OBJS=spmat.o
LIB=libuy.a

all: bfs transpose

bfs: bfs.c $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

transpose: transpose.c $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

lib: $(LIB)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

.PHONY: all clean purge

clean:
	rm -rf $(OBJS) *.dSYM

purge: clean
	rm -rf $(LIB) bfs transpose
