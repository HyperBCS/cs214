#ifndef _mymalloc_h_
#define _mymalloc_h_

#include <stdlib.h>
#include <stdio.h>
#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
#define free( x ) myfree( x, __FILE__, __LINE__ )

void *mymalloc(size_t size, char* file, int line);
void myfree(void *ptr, char* file, int line);
void listmem();

#endif