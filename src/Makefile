CC=gcc
CFLAGS=-Wall -Werror -g
LDFLAGS=-lm

all: Broadcast clean

Broadcast : Main.o Simulator.o Fifo.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY : clean

clean :
	rm -rf *.o *~

cleanall: clean
	rm -rf Broadcast

