CC=g++
CFLAGS=-std=c++11 -Wall -pedantic -O2
LIBS=-lm rscode-1.3/berlekamp.c rscode-1.3/crcgen.c rscode-1.3/galois.c rscode-1.3/rs.c

all: bms2A bms2B

bms2A: bms2A.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

bms2B: bms2B.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

clean:
	rm -f *.o bms2A bms2B
