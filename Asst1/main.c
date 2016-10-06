#include "mymalloc.h"

typedef struct memory_block {
	unsigned short int size;
	//short int allocated;
	//struct memory_block *next;
} mem_block;

int main(){
	printf("SIZE: %d\n",sizeof(mem_block));
	return 0;
}