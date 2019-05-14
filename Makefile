CC = gcc
CFLAGS = -lncurses
DFLAGS = -g
DEPS = infinistack.h csnek.h

all: clean csnek

csnek: csnek.o infinistack.o
	$(CC) -o csnek csnek.o infinistack.o $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm *.o csnek 2> /dev/null; true

debug: CFLAGS += $(DFLAGS)
debug: clean csnek
