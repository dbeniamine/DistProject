CC=gcc
CFLAGS=-Wall -Werror -g
LDFLAGS=-lm

all: Broadcast clean

Broadcast : Main.o Simulator.o Fifo.o SortedList.o Message.o Broadcast.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY : clean

clean :
	rm -rf *.o *~

cleanall: clean
	rm -rf Broadcast

