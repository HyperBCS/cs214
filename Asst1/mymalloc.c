#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>

static char total_memory[5000];

typedef struct memory_block {
	int size;
	int allocated;
	struct memory_block *next;
} mem_block;

static mem_block *head = (mem_block*) total_memory;

int foo2(int i){
	i = i*i;
	return i;
}

void listmem(){
	mem_block* ptr = head;
	while(ptr != 0){
		printf("SIZE: %d ALLOCATED: %d\n",ptr->size,ptr->allocated);
		//printf("NEXT: %p\n",ptr->next);
		ptr = ptr->next;
	}
}

void *mymalloc(int len, char* file, int line){
	static int defined;
	if(defined == 0){
		head->allocated = 0;
		head->size = 5000 - sizeof(mem_block);
		head->next = 0;
		defined = 1;
	}
	mem_block* ptr = head;
	while(1){
		if(ptr->allocated == 0 && ptr->size>= len){
		break;
		} else{
			ptr = ptr->next;
			if(ptr == 0){
				printf("NOT ENOUGH SPACE!!\n");
				return 0;
			}
		}
	}
	ptr->allocated = 1;
	int temp_size = ptr->size;
	ptr->size = len;
	if(ptr->size != temp_size){
		mem_block *node = (mem_block*)((char*)ptr+sizeof(mem_block)+ptr->size);
		node->allocated = 0;
		if(ptr->next == 0){
			node->size = (int)((char*)head+5000-(char*)node);
			ptr->next = node;
		} else{
			node->size = (int)((char*)node->next - (char*)node+node->size+sizeof(mem_block));
			node->next = ptr->next;
			ptr->next = node;
		}
	}

	return ptr;
}