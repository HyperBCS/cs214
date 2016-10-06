#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>

int foo2(int i){
	i = i*i;
	return i;
}

void *mymalloc(int size, char* file, int line){
	return 0;
}