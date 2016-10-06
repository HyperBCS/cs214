#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>

static char total_memory[5000];

typedef struct memory_block {
	int size;
	int allocated;
	int address;
	struct memory_block *next;
} mem_block;

int foo2(int i){
	i = i*i;
	return i;
}

void *mymalloc(int size, char* file, int line){
	static mem_block block = (mem_block*) total_memory; 
	return *total_memory;
}