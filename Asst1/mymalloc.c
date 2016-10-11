#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>

#define NEXT (mem_block*)((char*)ptr+sizeof(mem_block)+(ptr->size & ~1))
#define NEXT_NEXT (mem_block*)((char*)next+sizeof(mem_block)+(next->size & ~1))
#define MAX_SIZE 12000

static char total_memory[MAX_SIZE];
int block_size = MAX_SIZE;


typedef struct memory_block {
	unsigned short int size;
} mem_block;

static mem_block *head = (mem_block*) total_memory;

void listmem(){
	mem_block*  end = (mem_block*)((char*)head + block_size);
	mem_block* ptr = head;
	while(ptr < end){
		printf("SIZE: %d ALLOCATED: %d\n",ptr->size & ~1,ptr->size & 1);
		ptr = NEXT;
	}
	if(end-ptr == 0){
		printf("SIZES MATCH\n");
	} else{
		printf("SIZE MISMATCH\n %d\n",(int)(end-ptr));

	}
}

void *mymalloc(int len, char* file, int line){
	if(len <= 0){
		return 0;
	} else if(len % 2 == 1){
		len = len + 1;
	}

	static int defined;
	if(defined == 0){
		head->size = block_size - sizeof(mem_block);
		defined = 1;
	}
	mem_block* ptr = head;
	mem_block*  end = (mem_block*)((char*)head + block_size);
	while(1){
		if((ptr->size & 1) == 0 && ptr->size>= len){
		break;
		} else{
			ptr = NEXT;
			if(ptr > end){
				printf("NOT ENOUGH SPACE!! | FILE: %s | LINE: %d\n", file, line);
				return 0;
			}
		}
	}

	mem_block *node = (mem_block*)((char*)ptr+sizeof(mem_block)+len);
	if(end-node<0){
		printf("NO ROOM | FILE: %s | LINE: %d\n", file, line);
		return 0;
	}
	if(NEXT == end){
		node->size = (int)((char*)head+block_size-(char*)node-sizeof(mem_block));
	} else{
		int size = (int)((char*)NEXT - (char*)ptr-len-sizeof(mem_block));
		if(size > 0){
		node->size = (int)((char*)NEXT - (char*)ptr-len-2*sizeof(mem_block));	
		}
	}
	//IF YOU DONT WANT TO INCLUDE 0 SIZE MALLOC UNCOMMENT. OTHERWISE IT WILL ADD TO END OF POINTER.
	if(node->size == 0){
		len = len+sizeof(mem_block);
	}

	ptr->size = len;
	ptr->size = ptr->size + 1;
	return ptr+1;
}

void myfree(void *point, char* file, int line){
	if(point == 0){
		printf("TRIED TO FREE NULL POINTER | FILE: %s | LINE: %d\n", file, line);
		return;
	}
	mem_block * node = point-sizeof(mem_block);
	mem_block * end = (mem_block*)((char*)head + block_size);
	mem_block* find = head;
	int found = 0;
	while(find < end){
		if(find == node){
			found = 1;
		}
		find = (mem_block*)((char*)find+sizeof(mem_block)+(find->size & ~1));
	}
	if(found == 0){
		printf("POINTER NOT A MALLOC ADDRESS | FILE: %s | LINE: %d\n", file, line);
		return;
	} else if(node->size < 0 || node->size > block_size){
		printf("ADDRESS CORRUPT | FILE: %s | LINE: %d\n", file, line);
	}
	if((node->size & 1) == 1){
		node->size = node->size - 1;
	} else{
		printf("POINTER ALREADY FREED | FILE: %s | LINE: %d\n", file, line);
		return;
	}
	mem_block *prev = 0;
	mem_block *ptr = head;
	while(ptr != node){
		prev = ptr;
		ptr = NEXT;
	}
	mem_block *next = NEXT;
	if(prev != 0){
		// prev->alloc = 0 next-alloc = NULL
		if(NEXT >= end && (prev->size & 1) == 0){
			//printf("Case 1\n");
			prev->size = (int)((char*)head+block_size-(char*)prev-sizeof(mem_block));
		// prev->alloc = 0 next-alloc = 1
		} else if((next->size & 1) != 0 && (prev->size & 1) == 0){
			//printf("Case 2\n");
			prev->size = (int)((char*)next - (char*)prev-sizeof(mem_block));
		// prev->alloc = 1 next-alloc = 0 next->next = null
		} else if((next->size & 1) == 0 && (prev->size & 1) != 0 && NEXT_NEXT >= end){
			//printf("Case 3\n");
			ptr->size = (int)((char*)head+block_size-(char*)ptr-sizeof(mem_block));
		// prev->alloc = 0 next-alloc = 0 next->next = null
		} else if((next->size & 1) == 0 && (prev->size & 1) == 0 && NEXT_NEXT >= end){ 
			//printf("Case 4\n");
			prev->size = (int)((char*)head+block_size - (char*)prev-sizeof(mem_block));
		// prev->alloc = 0 next-alloc = 0
		} else if((next->size & 1) == 0 && (prev->size & 1) == 0){
			//printf("Case 5\n");
			prev->size = (int)((char*)NEXT_NEXT - (char*)prev-sizeof(mem_block));
		// prev-> alloc = 1 next->alloc = 0
		} else if((next->size & 1) == 0 && (prev->size & 1) != 0){
			//printf("Case 6\n");
			ptr->size = (int)((char*)NEXT_NEXT - (char*)ptr-sizeof(mem_block));
		} else{
			//printf("NO CASE!\n");
		}
	} else{
		// prev->alloc = null next-alloc = 0
		 if((next->size & 1) == 0 && NEXT_NEXT >= end){
			//printf("Case 7\n");
			ptr->size = (int)((char*)head+block_size-(char*)ptr-sizeof(mem_block));
		// prev->alloc = null next-alloc = 0
		} else if((next->size & 1) == 0){
			//printf("Case 8\n");
			ptr->size = (int)((char*)NEXT_NEXT-(char*)ptr-sizeof(mem_block));
		} else{
			//printf("NO CASE!\n");
		}
	}
}