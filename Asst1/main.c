#include "mymalloc.h"

int main(){
int * a = malloc(1000);
int * b = malloc(1000);
int * c = malloc(1000);
int * d = malloc(1000);
int * e = malloc(990);
//free(c);
free(a);
a = malloc(500);
free(c);
free(b);
free(e);
free(d);
free(a);
listmem();
	return 0;
}