CC=g++
CFLAGS=-std=c++11 -Wall -pedantic -O2
LIBS=-lm

all: bms2A bms2B


bms2A: bms2A.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

bms2B: bms2B.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

clean:
	rm -f *.o bms2A bms2B
