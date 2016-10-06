#ifndef _mymalloc_h_
#define _mymalloc_h_

#include <stdlib.h>
#include <stdio.h>
#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
#define foo( x ) foo2( x, __FILE__, __LINE__ )
//#define free( x ) myfree( x, __FILE__, __LINE__ )

int foo2(int i);

void *mymalloc(int size, char* file, int line);
void listmem();

#endif