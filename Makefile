CC=gcc-11
CFLAGS=-Wall -Werror -O2
# CFLAGS=-Wall -Werror -g -O0 -fsanitize=address -fno-omit-frame-pointer
INCLUDES=-I./include
AR=ar
ARFLAGS=rcs
OBJS=src/spmat.o
PRGS=bfs transpose write_mtx
LIB=libuy.a

all: $(PRGS)

bfs: prgs/bfs.o $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

transpose: prgs/transpose.o $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

write_mtx: prgs/write_mtx.o $(LIB)
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
