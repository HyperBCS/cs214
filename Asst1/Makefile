CC = gcc
CCFLAGS = -Wall -g

all: build

build: mymalloc.o memgrind.o
	$(CC) $(CCFLAGS) -o build mymalloc.o memgrind.o

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CCFLAGS) -c mymalloc.c

memgrind.o: memgrind.c
	$(CC) $(CCFLAGS) -c memgrind.c

clean:
	rm -f build mymalloc.o memgrind.o