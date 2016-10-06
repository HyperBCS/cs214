#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>

static char total_memory[5000];
int block_size = 5000;


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

// void listmem(){
// 	mem_block* ptr = head;
// 	while(ptr != 0){
// 		printf("SIZE: %d ALLOCATED: %d\n",ptr->size,ptr->allocated);
// 		//printf("NEXT: %p\n",ptr->next);
// 		ptr = ptr->next;
// 	}
// }

void listmem(){
	mem_block*  end = (mem_block*)((char*)head + block_size);
	mem_block* ptr = head;
	while(ptr < end){
		printf("SIZE: %d ALLOCATED: %d\n",ptr->size,ptr->allocated);
		//printf("NEXT: %p\n",ptr->next);
		ptr = (mem_block*)((char*)ptr+sizeof(mem_block)+ptr->size);
	}
	if(end-ptr == 0){
		printf("SIZES MATCH\n");
	} else{
		printf("SIZE MISMATCH\n %d\n",end-ptr);

	}
}

void *mymalloc(int len, char* file, int line){
	if(len == 0){
		return head;
	}
	static int defined;
	if(defined == 0){
		head->allocated = 0;
		head->size = block_size - sizeof(mem_block);
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
	if(len != ptr->size){
		mem_block *node = (mem_block*)((char*)ptr+sizeof(mem_block)+len);
		node->allocated = 0;
		if(ptr->next == 0){
			node->size = (int)((char*)head+block_size-(char*)node-sizeof(mem_block));
		} else{
			node->size = (int)((char*)ptr->next - (char*)ptr-len-sizeof(mem_block));
			node->next = ptr->next;
		}
		if(node->size >0){
		ptr->next = node;
		} else if(node->size < 0){
			printf("NO ROOM\n");
			return 0;
		}
	ptr->size = len;
	ptr->allocated = 1;
	}
	return ptr+1;
}

void myfree(void *point, char* file, int line){
	if(point == 0){
		printf("NULL POINTER\n");
		return;
	}
	mem_block *node = point-sizeof(mem_block);
	if(node->allocated == 1){
		node->allocated = 0;
	} else if(node->allocated != 1 || node->allocated != 0){
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
		ptr = ptr->next;
	}
	if(prev != 0){
		// prev->alloc = 0 next-alloc = NULL
		if(ptr->next == 0 && prev->allocated == 0){
			//printf("Case 1\n");
			prev->size = (int)((char*)head+block_size-(char*)prev-sizeof(mem_block));
			prev->next = ptr->next;
		// prev->alloc = 0 next-alloc = 1
		} else if(ptr->next->allocated != 0 && prev->allocated == 0){
			//printf("Case 2\n");
			prev->size = (int)((char*)ptr->next - (char*)prev-sizeof(mem_block));
			prev->next = ptr->next;
		// prev->alloc = 1 next-alloc = 0 next->next = null
		} else if(ptr->next->allocated == 0 && prev->allocated != 0 && ptr->next->next == 0){
			//printf("Case 3\n");
			ptr->size = (int)((char*)head+block_size-(char*)ptr-sizeof(mem_block));
			ptr->next = 0;
		// prev->alloc = 0 next-alloc = 0 next->next = null
		} else if(ptr->next->allocated == 0 && prev->allocated == 0 && ptr->next->next == 0){ 
			//printf("Case 4\n");
			prev->size = (int)((char*)head+block_size - (char*)prev-sizeof(mem_block));
			prev->next = 0;
		// prev->alloc = 0 next-alloc = 0
		} else if(ptr->next->allocated == 0 && prev->allocated == 0){
			//printf("Case 5\n");
			prev->size = (int)((char*)ptr->next->next - (char*)prev-sizeof(mem_block));
			prev->next = ptr->next->next;
		// prev-> alloc = 1 next->alloc = 0
		} else if(ptr->next->allocated == 0 && prev->allocated != 0){
			//printf("Case 6\n");
			ptr->size = (int)((char*)ptr->next->next - (char*)ptr-sizeof(mem_block));
			ptr->next = ptr->next->next;
		} else{
			//printf("NO CASE!\n");
		}
	} else{
		// prev->alloc = null next-alloc = 0
		 if(ptr->next->allocated == 0 && ptr->next->next == 0){
			//printf("Case 7\n");
			ptr->next = 0;
			ptr->size = (int)((char*)head+block_size-(char*)ptr-sizeof(mem_block));
		// prev->alloc = null next-alloc = 0
		} else if(ptr->next->allocated == 0){
			//printf("Case 8\n");
			ptr->size = (int)((char*)ptr->next->next-(char*)ptr-sizeof(mem_block));
			ptr->next = ptr->next->next;
		} else{
			//printf("NO CASE!\n");
		}
	}
}