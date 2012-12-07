CC=gcc
CFLAGS=-Wall -Werror -g
LDFLAGS=-lm

all: Broadcast

Broadcast : Main.o Simulator.o Fifo.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY : clean

clean :
	rm *.o Broadcast

