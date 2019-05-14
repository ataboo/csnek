CC=gcc
CFLAGS=-lncurses
DEPS=infinistack.h csnek.h

csnek: csnek.o infinistack.o
	$(CC) -o csnek csnek.o infinistack.o $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm *.o csnek
