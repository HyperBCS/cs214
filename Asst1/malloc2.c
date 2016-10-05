void* malloc(size_t size){
// Ask the system for more bytes by extending the heap space
// sbrk Returns -1 on failure
	void *p = sbrk(size);
	if(p == (void *) -1) return NULL; // No space left
	return p;
}

void free() {/* Do nothing*/}

// Traverse linked list to find free unallocated block
	// if size < p.size
	// create node -size
					
	//insert node before p
					update p size->p.size 
	// else return null