#include "mymalloc.h"

// Set macro to cleanup code and MAX_SIZE of block
#define NEXT (mem_block*)((char*)ptr+sizeof(mem_block)+(ptr->size & ~1))
#define NEXT_NEXT (mem_block*)((char*)next+sizeof(mem_block)+(next->size & ~1))
#define MAX_SIZE 5000

// Create initial block and set max size
static char total_memory[MAX_SIZE];
int block_size = MAX_SIZE;

// Metadata for each block
typedef struct memory_block {
	unsigned short int size;
} mem_block;

// Set a pointer to the start of the block
static mem_block *head = (mem_block*) total_memory;

// A method which prints the contents of the memory block
void listmem(){
	mem_block * end = (mem_block*)((char*)head + block_size);
	mem_block * ptr = head;
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

//Method for acutal mallocing
void *mymalloc(size_t len, char* file, int line){
	// Error check for size > max_size and of size <= 0
	if(len <= 0){
		return 0;
	} else if(len % 2 == 1){
		len = len + 1;
	}
	// Check if we defined the first block yet
	static int defined;
	if(defined == 0){
		head->size = block_size - sizeof(mem_block);
		defined = 1;
	}
	// Define head and end blocks
	mem_block* ptr = head;
	mem_block* end = (mem_block*)((char*)head + block_size);
	// Check if there is any available space in block
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
	// Create a pointer to the next free block
	mem_block *node = (mem_block*)((char*)ptr+sizeof(mem_block)+len);
	// If block is placed after end there is no room and we should quit
	if(end-node<0){
		printf("NO ROOM | FILE: %s | LINE: %d\n", file, line);
		return 0;
	}
	// Here we calculate the size of the next free block 
	// If next == end then we know the next node is end
	if(NEXT == end){
		node->size = (int)((char*)head+block_size-(char*)node-sizeof(mem_block));
	} else{
	// if next node is not end we need to subtract free space
		int size = (int)((char*)NEXT - (char*)ptr-len-sizeof(mem_block));
		if(size > 0){
			node->size = (int)((char*)NEXT - (char*)ptr-len-2*sizeof(mem_block));	
		}
	}
	//IF YOU DONT WANT TO INCLUDE 0 SIZE MALLOC UNCOMMENT. OTHERWISE IT WILL ADD TO END OF POINTER.
	if(node->size == 0){
		len = len+sizeof(mem_block);
	}
	// Give allocated memory its size and allocate it by changing first bit
	ptr->size = len;
	ptr->size = ptr->size + 1;
	// Add 1 so we can return address to where data starts
	return ptr+1;
}

// Method for freeing memory
void myfree(void *point, char* file, int line){
	// If trying to free null pointer check
	if(point == 0){
		printf("TRIED TO FREE NULL POINTER | FILE: %s | LINE: %d\n", file, line);
		return;
	}
	// Setup the pointer to correct locations
	mem_block * node = point-sizeof(mem_block);
	mem_block * end = (mem_block*)((char*)head + block_size);
	mem_block * find = head;
	// Check here if the pointer exists by going through all pointers and checking for the address
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
	// Begin a search through the implicit linked list to find the block just freed, then through a combination of if statements combine surroundin free blocks
	mem_block *prev = 0;
	mem_block *ptr = head;
	while(ptr != node){
		prev = ptr;
		ptr = NEXT;
	}
	// Use the definion in header
	mem_block *next = NEXT;
	if(prev != 0){
		// prev->alloc = 0 next-alloc = NULL
		if(NEXT >= end && (prev->size & 1) == 0){
			prev->size = (int)((char*)head+block_size-(char*)prev-sizeof(mem_block));
		// prev->alloc = 0 next-alloc = 1
		} else if((next->size & 1) != 0 && (prev->size & 1) == 0){
			prev->size = (int)((char*)next - (char*)prev-sizeof(mem_block));
		// prev->alloc = 1 next-alloc = 0 next->next = null
		} else if((next->size & 1) == 0 && (prev->size & 1) != 0 && NEXT_NEXT >= end){
			ptr->size = (int)((char*)head+block_size-(char*)ptr-sizeof(mem_block));
		// prev->alloc = 0 next-alloc = 0 next->next = null
		} else if((next->size & 1) == 0 && (prev->size & 1) == 0 && NEXT_NEXT >= end){ 
			prev->size = (int)((char*)head+block_size - (char*)prev-sizeof(mem_block));
		// prev->alloc = 0 next-alloc = 0
		} else if((next->size & 1) == 0 && (prev->size & 1) == 0){
			prev->size = (int)((char*)NEXT_NEXT - (char*)prev-sizeof(mem_block));
		// prev-> alloc = 1 next->alloc = 0
		} else if((next->size & 1) == 0 && (prev->size & 1) != 0){
			ptr->size = (int)((char*)NEXT_NEXT - (char*)ptr-sizeof(mem_block));
		} else{
		}
	} else{
		// prev->alloc = null next-alloc = 0
		 if((next->size & 1) == 0 && NEXT_NEXT >= end){
			ptr->size = (int)((char*)head+block_size-(char*)ptr-sizeof(mem_block));
		// prev->alloc = null next-alloc = 0
		} else if((next->size & 1) == 0){
			ptr->size = (int)((char*)NEXT_NEXT-(char*)ptr-sizeof(mem_block));
		} else{
		}
	}
}