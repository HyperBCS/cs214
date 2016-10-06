#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>

#define NEXT (mem_block*)((char*)ptr+sizeof(mem_block)+ptr->size)
#define NEXT_NEXT (mem_block*)((char*)next+sizeof(mem_block)+next->size)

static char total_memory[5000];
int block_size = 5000;


typedef struct memory_block {
	unsigned short int size;
	short int allocated;
} mem_block;

static mem_block *head = (mem_block*) total_memory;

int foo2(int i){
	i = i*i;
	return i;
}

void listmem(){
	mem_block*  end = (mem_block*)((char*)head + block_size);
	mem_block* ptr = head;
	while(ptr < end){
		printf("SIZE: %d ALLOCATED: %d\n",ptr->size,ptr->allocated);
		//printf("NEXT: %p\n",ptr->next);
		ptr = NEXT;
	}
	if(end-ptr == 0){
		printf("SIZES MATCH\n");
	} else{
		printf("SIZE MISMATCH\n %d\n",(int)(end-ptr));

	}
}

void *mymalloc(int len, char* file, int line){
	if(len == 0){
		return head;
	} else if(len % 2 == 1){
		len = len + 1;
	}
	static int defined;
	if(defined == 0){
		head->allocated = 0;
		head->size = block_size - sizeof(mem_block);
		defined = 1;
	}
	mem_block* ptr = head;
	mem_block*  end = (mem_block*)((char*)head + block_size);
	while(1){
		if(ptr->allocated == 0 && ptr->size>= len){
		break;
		} else{
			ptr = NEXT;
			if(ptr > end){
				printf("NOT ENOUGH SPACE!!\n");
				return 0;
			}
		}
	}
	if(len != ptr->size){
		mem_block *node = (mem_block*)((char*)ptr+sizeof(mem_block)+len);
		node->allocated = 0;
		if(NEXT == end){
			node->size = (int)((char*)head+block_size-(char*)node-sizeof(mem_block));
		} else{
			node->size = (int)((char*)NEXT - (char*)ptr-len-sizeof(mem_block));
		}
		if(node->size >0){
		} else if(node->size < 0){
			printf("NO ROOM\n");
			return 0;
		}
	ptr->size = len;
	ptr->allocated = 1;
	if(ptr->allocated & 1){
		printf("EXTRACTED BIT\n");
	}
	}
	return ptr+1;
}

void myfree(void *point, char* file, int line){
	if(point == 0){
		printf("NULL POINTER\n");
		return;
	}
	mem_block * node = point-sizeof(mem_block);
	mem_block * end = (mem_block*)((char*)head + block_size);
	if(node->allocated == 1){
		node->allocated = 0;
	} else if(node->allocated != 1 && node->allocated != 0){
		printf("POINTER NOT A MALLOC ADDRESS\n");
		return;
	}
	else{
		printf("POINTER ALREADY FREED\n");
		return;
	}
	mem_block *prev = 0;
	mem_block *ptr = head;
	while(ptr != node){
		prev = ptr;
		ptr = NEXT;
	}
	mem_block *next = NEXT;
	mem_block *next_next= NEXT_NEXT;
	if(prev != 0){
		// prev->alloc = 0 next-alloc = NULL
		if(NEXT >= end && prev->allocated == 0){
			//printf("Case 1\n");
			prev->size = (int)((char*)head+block_size-(char*)prev-sizeof(mem_block));
		// prev->alloc = 0 next-alloc = 1
		} else if(next->allocated != 0 && prev->allocated == 0){
			//printf("Case 2\n");
			prev->size = (int)((char*)next - (char*)prev-sizeof(mem_block));
		// prev->alloc = 1 next-alloc = 0 next->next = null
		} else if(next->allocated == 0 && prev->allocated != 0 && NEXT_NEXT >= end){
			//printf("Case 3\n");
			ptr->size = (int)((char*)head+block_size-(char*)ptr-sizeof(mem_block));
		// prev->alloc = 0 next-alloc = 0 next->next = null
		} else if(next->allocated == 0 && prev->allocated == 0 && NEXT_NEXT >= end){ 
			//printf("Case 4\n");
			prev->size = (int)((char*)head+block_size - (char*)prev-sizeof(mem_block));
		// prev->alloc = 0 next-alloc = 0
		} else if(next->allocated == 0 && prev->allocated == 0){
			//printf("Case 5\n");
			prev->size = (int)((char*)NEXT_NEXT - (char*)prev-sizeof(mem_block));
		// prev-> alloc = 1 next->alloc = 0
		} else if(next->allocated == 0 && prev->allocated != 0){
			//printf("Case 6\n");
			ptr->size = (int)((char*)NEXT_NEXT - (char*)ptr-sizeof(mem_block));
		} else{
			//printf("NO CASE!\n");
		}
	} else{
		// prev->alloc = null next-alloc = 0
		 if(next->allocated == 0 && NEXT_NEXT >= end){
			//printf("Case 7\n");
			ptr->size = (int)((char*)head+block_size-(char*)ptr-sizeof(mem_block));
		// prev->alloc = null next-alloc = 0
		} else if(next->allocated == 0){
			//printf("Case 8\n");
			ptr->size = (int)((char*)NEXT_NEXT-(char*)ptr-sizeof(mem_block));
		} else{
			//printf("NO CASE!\n");
		}
	}
}