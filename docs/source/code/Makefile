# Tested with Ubuntu 17.04, GNU Make 4.1 and gcc 7.2.0

.DEFAULT_GOAL=all

CC=gcc
IDIR=-I/usr/include -I/usr/include/hiredis
CFLAGS=-O3 -Wall $(IDIR) -g

LDIR=/usr/lib/i386-linux-gnu
LIBS=-lhiredis
BINARIES=create-graph-01 create-topchart

all: $(BINARIES)

create-graph-01: create-graph-01.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

create-topchart: create-topchart.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o : %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.o *~ core *.csv

clean-all:
	rm -f *.o *~ core *.csv $(BINARIES)

# EOF
